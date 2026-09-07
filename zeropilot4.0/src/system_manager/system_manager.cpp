#include "system_manager.hpp"
#include "zp_params.hpp"
#include "flightmode.hpp"
#include "attitude_manager.hpp"
#include "telemetry_manager.hpp"

#define LOG_TIMING 0

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
        oldDataCount(0),
        rcConnected(false),
        rcChannelReversed{},
        batteryData({PMData_t{}, MAV_BATTERY_CHARGE_STATE_OK, 0, 0}),
        socEstimator(batteryData),
        profilerId(0),
        paramSetup(this)
{
    paramSetup.loadAllParams();
    paramSetup.bindAllParamCallbacks();
    systemUtilsDriver->profilerRegister("SM", &profilerId);
}

void SystemManager::smUpdate() {
    ZP_ERROR_e result = ZP_ERROR_OK;
    systemUtilsDriver->profilerBegin(profilerId);

    // Kick the watchdog
    result |= iwdgDriver->refreshWatchdog();

    // Update the state of the safety switch if the driver is available
    if (safetySwitchDriver != nullptr) {
        result |= safetySwitchUpdate();
    }


    // Get RC data from the RC receiver and passthrough to AM if new
    RCControl rcData;
    result |= rcDriver->getRCData(rcData);
    
    if (result == ZP_ERROR_OK) {
        if (rcData.isDataNew) {
            oldDataCount = 0;
            result |= sendRCDataToAttitudeManager(rcData);

            if (!rcConnected) {
                result |= sendStatusTextToTelemetryManager(MAV_SEVERITY_INFO, "RC Connected");
                // loggerDriver->log("RC Connected"); (TODO: Uncomment after rearchitecture)
                rcConnected = true;
            }
        } else {
            oldDataCount += 1;

            float fsTimeout = 0.0f;
            if (ZP_PARAM::get(ZP_PARAM_ID::RC_FS_TIMEOUT, fsTimeout) == ZP_ERROR_OK) {
                if ((oldDataCount * SM_UPDATE_LOOP_DELAY_MS > (fsTimeout * 1000)) && rcConnected) {
                    result |= sendStatusTextToTelemetryManager(MAV_SEVERITY_CRITICAL, "RC Disconnected");
                    // loggerDriver->log("RC Disconnected"); (TODO: Uncomment after rearchitecture)
                    rcConnected = false;
                }
            }
        }
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
    MAV_STATE systemStatus = MAV_STATE_ACTIVE;
    if (!rcConnected) {
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
                if (profiles[i].maxExecUs >= (SM_UPDATE_LOOP_DELAY_MS * 1000)) {
                    result |= sendStatusTextToTelemetryManager(MAV_SEVERITY_CRITICAL, "SM execution time exceeding scheduled rate");
                } else if (profiles[i].maxExecUs >= 0.8f * (SM_UPDATE_LOOP_DELAY_MS * 1000)) {
                    result |= sendStatusTextToTelemetryManager(MAV_SEVERITY_WARNING, "SM execution time about to exceed scheduled rate");
                }
            } else if (strcmp(profiles[i].name, "AM") == 0) {
                if (profiles[i].maxExecUs >= (AM_UPDATE_LOOP_DELAY_MS * 1000)) {
                    result |= sendStatusTextToTelemetryManager(MAV_SEVERITY_CRITICAL, "AM execution time exceeding scheduled rate");
                } else if (profiles[i].maxExecUs >= 0.8f * (AM_UPDATE_LOOP_DELAY_MS * 1000)) {
                    result |= sendStatusTextToTelemetryManager(MAV_SEVERITY_WARNING, "AM execution time about to exceed scheduled rate");
                }
            } else if (strcmp(profiles[i].name, "TM") == 0) {
                if (profiles[i].maxExecUs >= (TM_UPDATE_LOOP_DELAY_MS * 1000)) {
                    result |= sendStatusTextToTelemetryManager(MAV_SEVERITY_CRITICAL, "TM execution time exceeding scheduled rate");
                } else if (profiles[i].maxExecUs >= 0.8f * (TM_UPDATE_LOOP_DELAY_MS * 1000)) {
                    result |= sendStatusTextToTelemetryManager(MAV_SEVERITY_WARNING, "TM execution time about to exceed scheduled rate");
                }
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

    // Increment scheduling counter
    smSchedulingCounter = (smSchedulingCounter + 1) % SM_SCHEDULING_RATE_HZ;

    systemUtilsDriver->profilerEnd(profilerId);
}

ZP_ERROR_e SystemManager::safetySwitchUpdate() {
    ZP_ERROR_e result = ZP_ERROR_OK;

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

ZP_ERROR_e SystemManager::updateBatteryFSM() {
    ZP_ERROR_e result = ZP_ERROR_OK;
    batteryData.isValid = false;         
    MAV_BATTERY_CHARGE_STATE currentBatteryState;

    result |= pmDriver->readData(&batteryData.pmData);

    if (result == ZP_ERROR_OK) {
        batteryData.isValid = true;         
        currentBatteryState = batteryData.chargeState;

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

            // Logging --> once per transition, checks if the state has yet to be logged and does so 
            if (currentBatteryState != batteryData.chargeState) {
                switch (batteryData.chargeState) {
                    case MAV_BATTERY_CHARGE_STATE_OK:
                        result |= sendStatusTextToTelemetryManager(MAV_SEVERITY_INFO, "Battery State: OK");
                        break;
                    case MAV_BATTERY_CHARGE_STATE_LOW:
                        result |= sendStatusTextToTelemetryManager(MAV_SEVERITY_WARNING, "Battery State: LOW");
                        break;
                    case MAV_BATTERY_CHARGE_STATE_CRITICAL:
                        result |= sendStatusTextToTelemetryManager(MAV_SEVERITY_CRITICAL, "Battery State: CRITICAL");
                        break;
                    default:
                        break;
                }
            }
        }
    }

    return result;
}

ZP_ERROR_e SystemManager::sendRCDataToTelemetryManager(const RCControl &rcData) {
    ZP_ERROR_e result = ZP_ERROR_OK;
    TMMessage_t rcDataMsg;
    uint32_t currentTime = systemUtilsDriver->getCurrentTimestampMs();
    result |= rcDataPack(rcDataMsg, currentTime, rcData.controlSignals, INPUT_CHANNELS);
    
    if (result == ZP_ERROR_OK) {
        result |= tmQueue->push(&rcDataMsg);
    }
    return result;
}

ZP_ERROR_e SystemManager::sendHeartbeatDataToTelemetryManager(uint8_t baseMode, uint32_t customMode, MAV_STATE systemStatus) {
    ZP_ERROR_e result = ZP_ERROR_OK;
    TMMessage_t hbDataMsg;
    uint32_t currentTime = systemUtilsDriver->getCurrentTimestampMs();
    result |= heartbeatPack(hbDataMsg, currentTime, baseMode, customMode, systemStatus);
    
    if (result == ZP_ERROR_OK) {
        result |= tmQueue->push(&hbDataMsg);
    }
    return result;
}

ZP_ERROR_e SystemManager::sendRCDataToAttitudeManager(const RCControl &rcData) {
    RCMotorControlMessage_t rcDataMessage;
    FlightMode_e fltMode;

    ZP_ERROR_e result = decodeRawFlightMode(rcData.fltModeRaw, fltMode);

    if (result == ZP_ERROR_OK) {
        rcDataMessage.roll = rcChannelReversed[0] ? 100.0f - rcData.roll : rcData.roll;
        rcDataMessage.pitch = rcChannelReversed[1] ? 100.0f - rcData.pitch : rcData.pitch;
        rcDataMessage.throttle = rcChannelReversed[2] ? 100.0f - rcData.throttle : rcData.throttle;
        rcDataMessage.yaw = rcChannelReversed[3] ? 100.0f - rcData.yaw : rcData.yaw;
        rcDataMessage.arm = (rcData.arm > SM_RC_ARM_THRESHOLD) && !isSafetySwitchEngaged;
        #ifdef PLANE
        rcDataMessage.flapAngle = rcData.aux2;
        #endif
        rcDataMessage.flightMode = fltMode;

        result |= amRCQueue->push(&rcDataMessage);
    }
    return result;
}

ZP_ERROR_e SystemManager::sendBatteryDataToTelemetryManager(const BatteryData_t &batteryData, const uint8_t batteryId) {
    static constexpr uint8_t VOLTAGE_LEN = 1;
    float voltages[VOLTAGE_LEN] = {batteryData.pmData.busVoltage};

    TMMessage_t batteryDataMsg;
    uint32_t currentTime = systemUtilsDriver->getCurrentTimestampMs();
    ZP_ERROR_e result = batteryDataPack(batteryDataMsg, currentTime, batteryId,
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

ZP_ERROR_e SystemManager::sendStatusTextToTelemetryManager(MAV_SEVERITY severity, const char text[50], uint16_t id, uint8_t chunk_seq) {
    ZP_ERROR_e result = ZP_ERROR_OK;
    TMMessage_t statusTextMsg;
    uint32_t currentTime = systemUtilsDriver->getCurrentTimestampMs();
    result |= statusTextPack(statusTextMsg, currentTime, severity, text, id, chunk_seq);
    
    if (result == ZP_ERROR_OK) {
        result |= tmQueue->push(&statusTextMsg);
    }
    return result;
}

ZP_ERROR_e SystemManager::decodeRawFlightMode(float flightModeRawValue, FlightMode_e &outMode) {
    if (flightModeRawValue <= SM_FLIGHTMODE1_MAX) outMode = flightModes[0];
    else if (flightModeRawValue <= SM_FLIGHTMODE2_MAX) outMode = flightModes[1];
    else if (flightModeRawValue <= SM_FLIGHTMODE3_MAX) outMode = flightModes[2];
    else if (flightModeRawValue <= SM_FLIGHTMODE4_MAX) outMode = flightModes[3];
    else if (flightModeRawValue <= SM_FLIGHTMODE5_MAX) outMode = flightModes[4];
    else outMode = flightModes[5];

    return ZP_ERROR_OK;
}

ZP_ERROR_e SystemManager::sendMessagesToLogger() {
    ZP_ERROR_e result = ZP_ERROR_OK;
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