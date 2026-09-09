#include "system_manager.hpp"
#include "zp_params.hpp"
#include "flightmode.hpp"
#include "attitude_manager.hpp"
#include "telemetry_manager.hpp"

#define LOG_TIMING 0

static MAV_SEVERITY toMavSeverity(BitLevel_e level);

SystemManager::SystemManager(
    ISystemUtils *systemUtilsDriver,
    IIndependentWatchdog *iwdgDriver,
    ILogger *loggerDriver,
    ISafetySwitch *safetySwitchDriver,
    IRCReceiver *rcDriver,
    IPowerModule *pmDriver,
    IMessageQueue<RCMotorControlMessage_t> *amRCQueue,
    IMessageQueue<TMMessage_t> *tmQueue,
    IMessageQueue<char[100]> *smLoggerQueue) :
        systemUtilsDriver(systemUtilsDriver),
        iwdgDriver(iwdgDriver),
        loggerDriver(loggerDriver),
        safetySwitchDriver(safetySwitchDriver),
        rcDriver(rcDriver),
        pmDriver(pmDriver),
        amRCQueue(amRCQueue),
        tmQueue(tmQueue),
        smLoggerQueue(smLoggerQueue),
        smSchedulingCounter(0),
        flightModes{},
        isSafetySwitchEngaged(safetySwitchDriver == nullptr ? false : true),
        safetySwitchHoldCounterMs(0),
        safetySwitchTriggered(false),
        safetySwitchPrearmCntrMs(0),
        rcConnected(false),
        prevArmed(false),
        bitDisarmLatch(false),
        bitPrearmCntrMs(0),
        rcChannelReversed{},
        batteryData({PMData_t{}, MAV_BATTERY_CHARGE_STATE_OK, 0, 0}),
        socEstimator(batteryData),
        profilerId(0),
        paramSetup(this)
{
    paramSetup.loadAllParams();
    paramSetup.bindAllParamCallbacks();
    systemUtilsDriver->profilerRegister("SM", &profilerId);
    bindBitHandlers();
}

ZP_Error SystemManager::bindBitHandlers() {
    ZP_Error result = ZP_ERROR_OK;

    for (uint16_t i = 0; i < static_cast<uint16_t>(ZP_BIT_ID::NUM_BIT_IDS); i++) {
        // Catches the table drifting out of step with ZP_BIT_ID
        if (BIT_HANDLERS[i].id != static_cast<ZP_BIT_ID>(i)) {
            result |= ZP_ERROR_CONFIG;
            continue;
        }
        result |= ZP_BIT::bindHandler(static_cast<ZP_BIT_ID>(i), static_cast<void*>(this), BIT_HANDLERS[i].action);
    }

    // Preserve the timing of the failsafe this replaces
    float fsTimeout = 0.0f;
    if (ZP_PARAM::get(ZP_PARAM_ID::RC_FS_TIMEOUT, fsTimeout) == ZP_ERROR_OK) {
        result |= ZP_BIT::setPersistence(ZP_BIT_ID::RC_DATA_VALID,
                                         static_cast<uint32_t>(fsTimeout * 1000.0f),
                                         SM_UPDATE_LOOP_DELAY_MS * 3);
    }

    return result;
}

const SMBitHandler_t& SystemManager::bitRow(ZP_BIT_ID id) {
    static const SMBitHandler_t UNKNOWN_ROW = {
        ZP_BIT_ID::NUM_BIT_IDS, "Unknown BIT failed", SystemManager::reportBitCallback
    };

    if (static_cast<uint16_t>(id) >= static_cast<uint16_t>(ZP_BIT_ID::NUM_BIT_IDS)) {
        return UNKNOWN_ROW;
    }
    return BIT_HANDLERS[static_cast<uint16_t>(id)];
}

ZP_Error SystemManager::reportLoopTiming(ZP_BIT_ID id, uint32_t maxExecUs, uint32_t budgetMs) {
    const uint32_t BUDGET_US = budgetMs * 1000;

    // Fails once a loop is sustained at 80% of its budget, matching the threshold the old
    // "about to exceed scheduled rate" warning used
    ZP_Error timing = ZP_ERROR_OK;
    if (maxExecUs >= (BUDGET_US * 8) / 10) {
        timing |= ZP_ERROR_TIMEOUT;
    }

    (void)ZP_BIT::report(id, timing);
    return ZP_ERROR_OK;
}

void SystemManager::smUpdate() {
    ZP_Error result = ZP_ERROR_OK;
    systemUtilsDriver->profilerBegin(profilerId);

    // Kick the watchdog
    result |= iwdgDriver->refreshWatchdog();

    // Update the state of the safety switch if the driver is available
    if (safetySwitchDriver != nullptr) {
        result |= safetySwitchUpdate();
    }


    // Get RC data from the RC receiver and passthrough to AM if new.
    // Gate on this call's own status, not the tick-wide accumulator: bits from the watchdog or
    // safety switch above must not suppress RC passthrough, and they can no longer be cleared.
    RCControl rcData;
    ZP_Error rcStatus = rcDriver->getRCData(rcData);
    result |= rcStatus;

    // A stale frame is a health failure rather than a driver error. RC_DATA_VALID's debounce
    // window is seeded from RC_FS_TIMEOUT, so this replaces the old oldDataCount timer exactly.
    ZP_Error rcHealth = rcStatus;
    if (!rcData.isDataNew) {
        rcHealth |= ZP_ERROR_NOT_READY;
    }
    (void)ZP_BIT::report(ZP_BIT_ID::RC_DATA_VALID, rcHealth);

    BitState_e rcBitState = BitState_e::UNKNOWN;
    (void)ZP_BIT::getLive(ZP_BIT_ID::RC_DATA_VALID, rcBitState);
    rcConnected = (rcBitState == BitState_e::SUCCESS);

    if (rcStatus == ZP_ERROR_OK && rcData.isDataNew) {
        result |= sendRCDataToAttitudeManager(rcData);
    }

    // Send RC data to TM
    if (smSchedulingCounter % (SM_SCHEDULING_RATE_HZ / SM_TELEMETRY_RC_DATA_RATE_HZ) == 0) {
        result |= sendRCDataToTelemetryManager(rcData);
    }

    // Set armed status based on SM_RC_ARM_THRESHOLD
    bool armed = (rcData.arm > SM_RC_ARM_THRESHOLD) && !isSafetySwitchEngaged;

    // Populate baseMode based on arm state
    uint8_t baseMode = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED;
    if (armed) {
        baseMode |= MAV_MODE_FLAG_SAFETY_ARMED;
    }

    // Determine system status based on RC connection and arm state
    ZP_BIT_ID emergencyBit = ZP_BIT_ID::NUM_BIT_IDS;
    const bool BIT_BLOCKING = (ZP_BIT::prearmCheck(emergencyBit) != ZP_ERROR_OK);

    MAV_STATE systemStatus = MAV_STATE_ACTIVE;
    if (BIT_BLOCKING && armed) {
        systemStatus = MAV_STATE_EMERGENCY;
    } else if (!rcConnected) {
        systemStatus = MAV_STATE_CRITICAL;
    } else if (!armed) {
        systemStatus = MAV_STATE_STANDBY;
    }

    // Decode flight mode from raw value and include in custom mode for HEARTBEAT telemetry
    FlightMode_e flightMode;
    result |= decodeRawFlightMode(rcData.fltModeRaw, flightMode);
    uint32_t customMode = static_cast<uint32_t>(flightMode);

    // Send Heartbeat data to TM at a 1Hz rate
    if (smSchedulingCounter % (SM_SCHEDULING_RATE_HZ / SM_TELEMETRY_HEARTBEAT_RATE_HZ) == 0) {
        result |= sendHeartbeatDataToTelemetryManager(baseMode, customMode, systemStatus);
    }

    // Send SYS_STATUS sensor health to TM at a 1Hz rate
    if (smSchedulingCounter % (SM_SCHEDULING_RATE_HZ / SM_TELEMETRY_SYS_STATUS_RATE_HZ) == 0) {
        result |= sendSysStatusToTelemetryManager();
    }

    // Monitor Battery State and send Battery Data to TM at a 1Hz rate
    if (updateBatteryFSM() == ZP_ERROR_OK) {
        socEstimator.calcStateOfCharge(batteryData, SOC_CHARGE_DISCHARGE_MODE);
        if (smSchedulingCounter % (SM_SCHEDULING_RATE_HZ / SM_TELEMETRY_BATTERY_DATA_RATE_HZ) == 0) {
            result |= sendBatteryDataToTelemetryManager(batteryData, 0);
        }
    }

    // Log if new messages
    int counter = 0;
    result |= smLoggerQueue->count(counter);
    if (counter > 0 && result == ZP_ERROR_OK) {
        result |= sendMessagesToLogger();
    }

    // Send profiler stats at 1Hz
    if (smSchedulingCounter % (SM_SCHEDULING_RATE_HZ / SM_TELEMETRY_HEARTBEAT_RATE_HZ) == 0) {
        uint8_t count = 0;
        systemUtilsDriver->profilerGetAll(profiles, &count);

        for (uint8_t i = 0; i < count; i++) {
            if (strcmp(profiles[i].name, "SM") == 0) {
                result |= reportLoopTiming(ZP_BIT_ID::SM_LOOP_TIMING, profiles[i].maxExecUs, SM_UPDATE_LOOP_DELAY_MS);
            } else if (strcmp(profiles[i].name, "AM") == 0) {
                result |= reportLoopTiming(ZP_BIT_ID::AM_LOOP_TIMING, profiles[i].maxExecUs, AM_UPDATE_LOOP_DELAY_MS);
            } else if (strcmp(profiles[i].name, "TM") == 0) {
                result |= reportLoopTiming(ZP_BIT_ID::TM_LOOP_TIMING, profiles[i].maxExecUs, TM_UPDATE_LOOP_DELAY_MS);
            }
            #if LOG_TIMING
            snprintf((char*)profilerBuf, sizeof(profilerBuf), "%-12s %lu us      %lu hz", profiles[i].name, profiles[i].maxExecUs, profiles[i].avgRateHz);
            result |= sendStatusTextToTelemetryManager(MAV_SEVERITY_INFO, (char*)profilerBuf);
            #endif
        }
        #if LOG_TIMING
        result |= sendStatusTextToTelemetryManager(MAV_SEVERITY_INFO, "-------TASK TIMINGS-------");
        #endif
    }

    // A disarm is what clears latched faults, so a fault cannot silently un-block arming
    if (prevArmed && !armed) {
        result |= ZP_BIT::clearLatched();
        bitDisarmLatch = false;
    }
    prevArmed = armed;

    // Fire handlers for anything that changed state this tick
    result |= ZP_BIT::dispatch();

    // Re-nag about a blocking fault on the same interval the safety switch uses
    ZP_BIT_ID blockingBit = ZP_BIT_ID::NUM_BIT_IDS;
    if (ZP_BIT::prearmCheck(blockingBit) != ZP_ERROR_OK) {
        bitPrearmCntrMs += SM_UPDATE_LOOP_DELAY_MS;

        if (bitPrearmCntrMs >= (SM_SAFETY_SWITCH_PREARM_MSG_INTERVAL_S * 1000)) {
            bitPrearmCntrMs = 0;
            // Only CRITICAL BITs block arming, so the nag severity is fixed
            result |= sendStatusTextToTelemetryManager(MAV_SEVERITY_CRITICAL, bitRow(blockingBit).failText);
        }
    } else {
        bitPrearmCntrMs = 0;
    }

    // Increment scheduling counter
    smSchedulingCounter = (smSchedulingCounter + 1) % SM_SCHEDULING_RATE_HZ;

    systemUtilsDriver->profilerEnd(profilerId);
}

ZP_Error SystemManager::safetySwitchUpdate() {
    ZP_Error result = ZP_ERROR_OK;

    // Safety switch logic
    if (safetySwitchDriver->isSafetySwitchPressed()) {
        safetySwitchHoldCounterMs += SM_UPDATE_LOOP_DELAY_MS;

        // If held for threshold duration and not already triggered, toggle the safety switch state
        if (safetySwitchHoldCounterMs >= SM_SAFETY_SWITCH_HOLD_THRESHOLD_MS && !safetySwitchTriggered) {
            isSafetySwitchEngaged = !isSafetySwitchEngaged;
            safetySwitchTriggered = true;
        }
    } else {
        safetySwitchHoldCounterMs = 0;
        safetySwitchTriggered = false;
    }

    // Safety switch LED logic
    if (!isSafetySwitchEngaged) {
        safetySwitchDriver->setSafetySwitchLEDState(true);
    } else {
        if (smSchedulingCounter % (SM_SCHEDULING_RATE_HZ / SM_SAFETY_SWITCH_BLINK_RATE_HZ) == 0) {
            bool currentLedState = safetySwitchDriver->getSafetySwitchLEDState();
            safetySwitchDriver->setSafetySwitchLEDState(!currentLedState);
        }
    }

    // Handle "PreArm: Hardware Safety Switch" STATUSTEXT message
    if (isSafetySwitchEngaged) {
        safetySwitchPrearmCntrMs += SM_UPDATE_LOOP_DELAY_MS;

        if (safetySwitchPrearmCntrMs >= (SM_SAFETY_SWITCH_PREARM_MSG_INTERVAL_S * 1000)) {
            safetySwitchPrearmCntrMs = 0;
            result |= sendStatusTextToTelemetryManager(MAV_SEVERITY_CRITICAL, "PreArm: Hardware Safety Switch");
        }
    }
    return result;
}

ZP_Error SystemManager::updateBatteryFSM() {
    ZP_Error result = ZP_ERROR_OK;
    batteryData.isValid = false;         

    ZP_Error pmStatus = pmDriver->readData(&batteryData.pmData);
    result |= pmStatus;
    (void)ZP_BIT::report(ZP_BIT_ID::PM_DATA_VALID, pmStatus);

    if (result == ZP_ERROR_OK) {
        batteryData.isValid = true;         

        float lowVolt = 0.0f;
        float critVolt = 0.0f;
        float lowTimer = 0.0f;
        
        // Accumulate errors from parameter lookups
        result |= ZP_PARAM::get(ZP_PARAM_ID::BATT_LOW_VOLT, lowVolt);
        result |= ZP_PARAM::get(ZP_PARAM_ID::BATT_CRT_VOLT, critVolt);
        result |= ZP_PARAM::get(ZP_PARAM_ID::BATT_LOW_TIMER, lowTimer);
        uint32_t battLowTimeMs = static_cast<uint32_t>(lowTimer * 1000.0f);
        if (result == ZP_ERROR_OK) {
            if (batteryData.pmData.busVoltage >= lowVolt) {
                // Normal battery
                batteryData.chargeState = MAV_BATTERY_CHARGE_STATE_OK;
                batteryData.batteryLowCounterMs = 0;
                batteryData.batteryCritcounterMs = 0;
            } else if (batteryData.pmData.busVoltage >= critVolt) {
                // Low battery detection
                batteryData.batteryLowCounterMs += SM_UPDATE_LOOP_DELAY_MS;
                batteryData.batteryCritcounterMs = 0;
                if (battLowTimeMs > 0 && batteryData.batteryLowCounterMs >= battLowTimeMs) {
                    batteryData.chargeState = MAV_BATTERY_CHARGE_STATE_LOW;
                }
            } else {
                // Critical battery detection
                batteryData.batteryCritcounterMs += SM_UPDATE_LOOP_DELAY_MS;
                batteryData.batteryLowCounterMs = 0;
                if (battLowTimeMs > 0 && batteryData.batteryCritcounterMs >= battLowTimeMs) {
                    batteryData.chargeState = MAV_BATTERY_CHARGE_STATE_CRITICAL;
                }
            }

            // A severity ladder over two BITs: LOW is a warning, CRITICAL latches and blocks
            // arming. Both are monotone, so a critical battery fails BATT_LOW as well. The FSM
            // above has already applied BATT_LOW_TIMER, so both BITs use a zero debounce window.
            const bool IS_BATT_LOW = (batteryData.chargeState == MAV_BATTERY_CHARGE_STATE_LOW) ||
                                 (batteryData.chargeState == MAV_BATTERY_CHARGE_STATE_CRITICAL);
            const bool IS_BATT_CRITICAL = (batteryData.chargeState == MAV_BATTERY_CHARGE_STATE_CRITICAL);

            (void)ZP_BIT::report(ZP_BIT_ID::BATT_LOW, IS_BATT_LOW ? ZP_ERROR_INVALID_DATA : ZP_ERROR_OK);
            (void)ZP_BIT::report(ZP_BIT_ID::BATT_CRITICAL, IS_BATT_CRITICAL ? ZP_ERROR_INVALID_DATA : ZP_ERROR_OK);

        }
    }

    return result;
}

ZP_Error SystemManager::sendRCDataToTelemetryManager(const RCControl &rcData) {
    ZP_Error result = ZP_ERROR_OK;
    TMMessage_t rcDataMsg;
    uint32_t currentTime = systemUtilsDriver->getCurrentTimestampMs();
    result |= rcDataPack(rcDataMsg, currentTime, rcData.controlSignals, INPUT_CHANNELS);
    
    if (result == ZP_ERROR_OK) {
        result |= tmQueue->push(&rcDataMsg);
    }
    return result;
}

ZP_Error SystemManager::sendHeartbeatDataToTelemetryManager(uint8_t baseMode, uint32_t customMode, MAV_STATE systemStatus) {
    ZP_Error result = ZP_ERROR_OK;
    TMMessage_t hbDataMsg;
    uint32_t currentTime = systemUtilsDriver->getCurrentTimestampMs();
    result |= heartbeatPack(hbDataMsg, currentTime, baseMode, customMode, systemStatus);
    
    if (result == ZP_ERROR_OK) {
        result |= tmQueue->push(&hbDataMsg);
    }
    return result;
}

ZP_Error SystemManager::sendRCDataToAttitudeManager(const RCControl &rcData) {
    RCMotorControlMessage_t rcDataMessage;
    FlightMode_e fltMode;

    ZP_Error result = decodeRawFlightMode(rcData.fltModeRaw, fltMode);

    if (result == ZP_ERROR_OK) {
        rcDataMessage.roll = rcChannelReversed[0] ? 100.0f - rcData.roll : rcData.roll;
        rcDataMessage.pitch = rcChannelReversed[1] ? 100.0f - rcData.pitch : rcData.pitch;
        rcDataMessage.throttle = rcChannelReversed[2] ? 100.0f - rcData.throttle : rcData.throttle;
        rcDataMessage.yaw = rcChannelReversed[3] ? 100.0f - rcData.yaw : rcData.yaw;
        ZP_BIT_ID blockingBit = ZP_BIT_ID::NUM_BIT_IDS;
        const bool BIT_PREARM_OK = (ZP_BIT::prearmCheck(blockingBit) == ZP_ERROR_OK);

        // The safety-switch term is load bearing, and BIT is now the second gate
        rcDataMessage.arm = (rcData.arm > SM_RC_ARM_THRESHOLD) && !isSafetySwitchEngaged && BIT_PREARM_OK && !bitDisarmLatch;
        #ifdef PLANE
        rcDataMessage.flapAngle = rcData.aux2;
        #endif
        rcDataMessage.flightMode = fltMode;

        result |= amRCQueue->push(&rcDataMessage);
    }
    return result;
}

ZP_Error SystemManager::sendSysStatusToTelemetryManager() {
    ZP_Error result = ZP_ERROR_OK;

    uint32_t present = 0;
    uint32_t enabled = 0;
    uint32_t health = 0;
    result |= ZP_BIT::getHealthMask(present, enabled, health);

    TMMessage_t sysStatusMsg;
    uint32_t currentTime = systemUtilsDriver->getCurrentTimestampMs();
    result |= sysStatusPack(sysStatusMsg, currentTime, present, enabled, health,
                            0, // load: no CPU load measurement yet
                            batteryData.pmData.busVoltage,
                            batteryData.pmData.current,
                            static_cast<int8_t>(socEstimator.getSocPercentage()));

    if (result == ZP_ERROR_OK) {
        result |= tmQueue->push(&sysStatusMsg);
    }
    return result;
}

ZP_Error SystemManager::sendBatteryDataToTelemetryManager(const BatteryData_t &batteryData, const uint8_t batteryId) {
    static constexpr uint8_t VOLTAGE_LEN = 1;
    float voltages[VOLTAGE_LEN] = {batteryData.pmData.busVoltage};

    TMMessage_t batteryDataMsg;
    uint32_t currentTime = systemUtilsDriver->getCurrentTimestampMs();
    ZP_Error result = batteryDataPack(batteryDataMsg, currentTime, batteryId,
                                        batteryData.pmData.temperature, voltages, VOLTAGE_LEN,
                                        batteryData.pmData.current,
                                        batteryData.pmData.charge,
                                        batteryData.pmData.energy,
                                        socEstimator.getSocPercentage(),
                                        socEstimator.getTimeRemaining(),
                                        batteryData.chargeState);

    if (result == ZP_ERROR_OK) {
        result |= tmQueue->push(&batteryDataMsg);
    }
    return result;
}

ZP_Error SystemManager::sendStatusTextToTelemetryManager(MAV_SEVERITY severity, const char text[50], uint16_t id, uint8_t chunk_seq) {
    ZP_Error result = ZP_ERROR_OK;
    TMMessage_t statusTextMsg;
    uint32_t currentTime = systemUtilsDriver->getCurrentTimestampMs();
    result |= statusTextPack(statusTextMsg, currentTime, severity, text, id, chunk_seq);
    
    if (result == ZP_ERROR_OK) {
        result |= tmQueue->push(&statusTextMsg);
    }
    return result;
}

ZP_Error SystemManager::decodeRawFlightMode(float flightModeRawValue, FlightMode_e &outMode) {
    if (flightModeRawValue <= SM_FLIGHTMODE1_MAX) outMode = flightModes[0];
    else if (flightModeRawValue <= SM_FLIGHTMODE2_MAX) outMode = flightModes[1];
    else if (flightModeRawValue <= SM_FLIGHTMODE3_MAX) outMode = flightModes[2];
    else if (flightModeRawValue <= SM_FLIGHTMODE4_MAX) outMode = flightModes[3];
    else if (flightModeRawValue <= SM_FLIGHTMODE5_MAX) outMode = flightModes[4];
    else outMode = flightModes[5];

    return ZP_ERROR_OK;
}

ZP_Error SystemManager::sendMessagesToLogger() {
    ZP_Error result = ZP_ERROR_OK;
    static char messages[16][100];
    int msgCount = 0;
    int queueCount = 0;

    result |= smLoggerQueue->count(queueCount);

    if (result == ZP_ERROR_OK) {
        while (queueCount-- > 0) {
            result |= smLoggerQueue->get(&messages[msgCount]);
            if (result != ZP_ERROR_OK) break;
            msgCount++;
        }
    }
    return result;
}

void SystemManager::reportBitCallback(void* context, ZP_BIT_ID id, BitLevel_e level, BitState_e state) {
    SystemManager* ctx = static_cast<SystemManager*>(context);
    if (ctx == nullptr || state != BitState_e::FAILURE) {
        return;
    }

    (void)ctx->sendStatusTextToTelemetryManager(toMavSeverity(level), bitRow(id).failText);
}

void SystemManager::disarmBitCallback(void* context, ZP_BIT_ID id, BitLevel_e level, BitState_e state) {
    reportBitCallback(context, id, level, state);

    SystemManager* ctx = static_cast<SystemManager*>(context);
    if (ctx == nullptr || state != BitState_e::FAILURE) {
        return;
    }

    // Cleared only on the armed -> disarmed edge, so recovery alone cannot re-arm the aircraft
    ctx->bitDisarmLatch = true;
    (void)ctx->sendStatusTextToTelemetryManager(MAV_SEVERITY_EMERGENCY, "Disarming");
}

static MAV_SEVERITY toMavSeverity(BitLevel_e level) {
    return (level == BitLevel_e::CRITICAL) ? MAV_SEVERITY_CRITICAL : MAV_SEVERITY_WARNING;
}
