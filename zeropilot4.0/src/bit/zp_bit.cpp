#include "zp_bit.hpp"
#include "mavlink.h"

namespace ZP_BIT {

    namespace {

        typedef struct {
            const char* name;
            BitPhase_e  phase;
            BitLevel_e  level;
            uint32_t    failMs;       // continuous failure before live flips to FAILING
            uint32_t    clearMs;      // continuous success before live flips back to PASSING
            uint32_t    mavSensorBit; // MAV_SYS_STATUS_SENSOR_*, 0 if unmapped
            bool        blocksArming;
        } BitConfig_t;

        typedef struct {
            BitState_e     live;
            BitState_e     latched;
            BitState_e     runState;  // what the current uninterrupted run of reports says
            uint32_t       edgeMs;    // when that run began
            uint32_t       failMs;    // seeded from the table, overridable at runtime
            uint32_t       clearMs;
            bool           changed;   // live transitioned; consumed by dispatch()
            void*          context;
            BitHandlerCb_t onChange;
        } BitStatus_t;

        // Owner column records which manager reports each BIT. Exactly one writer per BIT is what
        // makes the lock-free table safe across the AM/SM/TM threads.
        //
        //  name                      phase                   level                failMs clearMs  mavSensorBit                              blocksArming   owner
        constexpr BitConfig_t BIT_CONFIG[static_cast<uint16_t>(ZP_BIT_ID::NUM_BIT_IDS)] = {
            {"PARAM_TABLE_INIT",  BitPhase_e::POWER_ON,   BitLevel_e::CRITICAL,      0,      0, 0,                                              true},  // initModel
            {"IMU_INIT",          BitPhase_e::POWER_ON,   BitLevel_e::CRITICAL,      0,      0, MAV_SYS_STATUS_SENSOR_3D_GYRO,                  true},  // initDrivers
            {"GPS1_INIT",         BitPhase_e::POWER_ON,   BitLevel_e::WARNING,       0,      0, MAV_SYS_STATUS_SENSOR_GPS,                      false}, // initDrivers
            {"GPS2_INIT",         BitPhase_e::POWER_ON,   BitLevel_e::WARNING,       0,      0, MAV_SYS_STATUS_SENSOR_GPS,                      false}, // initDrivers, H7 only
            {"BARO_INIT",         BitPhase_e::POWER_ON,   BitLevel_e::WARNING,       0,      0, MAV_SYS_STATUS_SENSOR_ABSOLUTE_PRESSURE,        false}, // initDrivers
            {"RC_INIT",           BitPhase_e::POWER_ON,   BitLevel_e::CRITICAL,      0,      0, MAV_SYS_STATUS_SENSOR_RC_RECEIVER,              true},  // initDrivers
            {"PM_INIT",           BitPhase_e::POWER_ON,   BitLevel_e::WARNING,       0,      0, MAV_SYS_STATUS_SENSOR_BATTERY,                  false}, // initDrivers
            {"TELEM_INIT",        BitPhase_e::POWER_ON,   BitLevel_e::WARNING,       0,      0, 0,                                              false}, // initDrivers
            {"RANGEFINDER_INIT",  BitPhase_e::POWER_ON,   BitLevel_e::WARNING,       0,      0, MAV_SYS_STATUS_SENSOR_LASER_POSITION,           false}, // initDrivers
            {"MOTOR_INIT",        BitPhase_e::POWER_ON,   BitLevel_e::CRITICAL,      0,      0, MAV_SYS_STATUS_SENSOR_MOTOR_OUTPUTS,            true},  // initDrivers
            {"CAN_INIT",          BitPhase_e::POWER_ON,   BitLevel_e::WARNING,       0,      0, 0,                                              false}, // initDrivers

            {"RC_DATA_VALID",     BitPhase_e::CONTINUOUS, BitLevel_e::CRITICAL,    500,    150, MAV_SYS_STATUS_SENSOR_RC_RECEIVER,              true},  // SM   @20Hz
            {"IMU_DATA_VALID",    BitPhase_e::CONTINUOUS, BitLevel_e::CRITICAL,     50,     50, MAV_SYS_STATUS_SENSOR_3D_GYRO,                  true},  // AM   @1kHz
            {"GPS_DATA_VALID",    BitPhase_e::CONTINUOUS, BitLevel_e::WARNING,    2000,    500, MAV_SYS_STATUS_SENSOR_GPS,                      false}, // AM   @1kHz
            {"BARO_DATA_VALID",   BitPhase_e::CONTINUOUS, BitLevel_e::WARNING,    1000,    500, MAV_SYS_STATUS_SENSOR_ABSOLUTE_PRESSURE,        false}, // AM   @1kHz
            {"PM_DATA_VALID",     BitPhase_e::CONTINUOUS, BitLevel_e::WARNING,    2000,    500, MAV_SYS_STATUS_SENSOR_BATTERY,                  false}, // SM   @20Hz
            {"RNGFND_DATA_VALID", BitPhase_e::CONTINUOUS, BitLevel_e::WARNING,    2000,    500, MAV_SYS_STATUS_SENSOR_LASER_POSITION,           false}, // AM   @1kHz
            {"TELEM_LINK_VALID",  BitPhase_e::CONTINUOUS, BitLevel_e::WARNING,    3000,   1000, 0,                                              false}, // TM   @20Hz
            {"BATT_LOW",          BitPhase_e::CONTINUOUS, BitLevel_e::WARNING,       0,      0, MAV_SYS_STATUS_SENSOR_BATTERY,                  false}, // SM   @20Hz
            {"BATT_CRITICAL",     BitPhase_e::CONTINUOUS, BitLevel_e::CRITICAL,      0,      0, MAV_SYS_STATUS_SENSOR_BATTERY,                  true},  // SM   @20Hz
            {"AM_LOOP_TIMING",    BitPhase_e::CONTINUOUS, BitLevel_e::WARNING,    2000,   2000, 0,                                              false}, // SM   @1Hz
            {"SM_LOOP_TIMING",    BitPhase_e::CONTINUOUS, BitLevel_e::WARNING,    2000,   2000, 0,                                              false}, // SM   @1Hz
            {"TM_LOOP_TIMING",    BitPhase_e::CONTINUOUS, BitLevel_e::WARNING,    2000,   2000, 0,                                              false}, // SM   @1Hz
        };

        BitStatus_t bitStatus[static_cast<uint16_t>(ZP_BIT_ID::NUM_BIT_IDS)];
        ISystemUtils* clockDriver = nullptr;

        constexpr uint16_t BIT_TOTAL = static_cast<uint16_t>(ZP_BIT_ID::NUM_BIT_IDS);

        inline bool indexValid(ZP_BIT_ID id) {
            return static_cast<uint16_t>(id) < BIT_TOTAL;
        }
    }

    ZP_Error init(ISystemUtils* clock) {
        if (clock == nullptr) {
            return ZP_ERROR_NULLPTR;
        }

        clockDriver = clock;

        for (uint16_t i = 0; i < BIT_TOTAL; i++) {
            bitStatus[i].live     = BitState_e::UNKNOWN;
            bitStatus[i].latched  = BitState_e::UNKNOWN;
            bitStatus[i].runState = BitState_e::UNKNOWN;
            bitStatus[i].edgeMs   = 0;
            bitStatus[i].failMs   = BIT_CONFIG[i].failMs;
            bitStatus[i].clearMs  = BIT_CONFIG[i].clearMs;
            bitStatus[i].changed  = false;
            bitStatus[i].context  = nullptr;
            bitStatus[i].onChange = nullptr;
        }

        return ZP_ERROR_OK;
    }

    ZP_Error report(ZP_BIT_ID id, ZP_Error status) {
        // Reporting must never alter the caller's status, even when BIT itself is misused
        if (!indexValid(id) || clockDriver == nullptr) {
            return status;
        }

        const BitConfig_t& config = BIT_CONFIG[static_cast<uint16_t>(id)];
        BitStatus_t& state = bitStatus[static_cast<uint16_t>(id)];

        const BitState_e OBSERVED = (status == ZP_ERROR_OK) ? BitState_e::SUCCESS : BitState_e::FAILURE;
        const uint32_t NOW = clockDriver->getCurrentTimestampMs();

        // A change of run restarts the debounce window
        if (state.runState != OBSERVED) {
            state.runState = OBSERVED;
            state.edgeMs = NOW;
        }

        // Unsigned subtraction, so the 49-day timestamp wrap is handled without a special case
        const uint32_t ELAPSED_MS = NOW - state.edgeMs;

        BitState_e newLive = state.live;
        if (OBSERVED == BitState_e::SUCCESS) {
            // A first-ever passing report resolves UNKNOWN immediately: the check demonstrably works
            if (state.live == BitState_e::UNKNOWN || ELAPSED_MS >= state.clearMs) {
                newLive = BitState_e::SUCCESS;
            }
        } else if (ELAPSED_MS >= state.failMs) {
            newLive = BitState_e::FAILURE;
        }

        if (newLive != state.live) {
            state.live = newLive;
            state.changed = true;

            if (newLive == BitState_e::FAILURE && config.level == BitLevel_e::CRITICAL) {
                state.latched = BitState_e::FAILURE;
            }
        }

        return status;
    }

    namespace {
        ZP_Error bindHandlerInternal(ZP_BIT_ID id, void* context, BitHandlerCb_t handler) {
            if (!indexValid(id)) {
                return ZP_ERROR_RANGE;
            }

            bitStatus[static_cast<uint16_t>(id)].context = context;
            bitStatus[static_cast<uint16_t>(id)].onChange = handler;
            return ZP_ERROR_OK;
        }
    }

    ZP_Error bindHandler(ZP_BIT_ID id, void* context, BitHandlerCb_t handler) {
        return bindHandlerInternal(id, context, handler);
    }

    ZP_Error dispatch() {
        for (uint16_t i = 0; i < BIT_TOTAL; i++) {
            if (!bitStatus[i].changed) {
                continue;
            }

            bitStatus[i].changed = false;

            if (bitStatus[i].onChange != nullptr) {
                bitStatus[i].onChange(bitStatus[i].context,
                                      static_cast<ZP_BIT_ID>(i),
                                      BIT_CONFIG[i].level,
                                      bitStatus[i].live);
            }
        }

        return ZP_ERROR_OK;
    }

    ZP_Error clearLatched() {
        for (uint16_t i = 0; i < BIT_TOTAL; i++) {
            bitStatus[i].latched = bitStatus[i].live;
        }

        return ZP_ERROR_OK;
    }

    ZP_Error setPersistence(ZP_BIT_ID id, uint32_t failMs, uint32_t clearMs) {
        if (!indexValid(id)) {
            return ZP_ERROR_RANGE;
        }

        bitStatus[static_cast<uint16_t>(id)].failMs = failMs;
        bitStatus[static_cast<uint16_t>(id)].clearMs = clearMs;
        return ZP_ERROR_OK;
    }

    ZP_Error getLive(ZP_BIT_ID id, BitState_e& outState) {
        if (!indexValid(id)) {
            outState = BitState_e::UNKNOWN;
            return ZP_ERROR_RANGE;
        }

        outState = bitStatus[static_cast<uint16_t>(id)].live;
        return ZP_ERROR_OK;
    }

    ZP_Error getLatched(ZP_BIT_ID id, BitState_e& outState) {
        if (!indexValid(id)) {
            outState = BitState_e::UNKNOWN;
            return ZP_ERROR_RANGE;
        }

        outState = bitStatus[static_cast<uint16_t>(id)].latched;
        return ZP_ERROR_OK;
    }

    ZP_Error prearmCheck(ZP_BIT_ID& outFirstBlocking) {
        for (uint16_t i = 0; i < BIT_TOTAL; i++) {
            if (!BIT_CONFIG[i].blocksArming) {
                continue;
            }

            // UNKNOWN never blocks: hardware that is absent on this airframe is simply never reported
            if (bitStatus[i].latched == BitState_e::FAILURE || bitStatus[i].live == BitState_e::FAILURE) {
                outFirstBlocking = static_cast<ZP_BIT_ID>(i);
                return ZP_ERROR_NOT_READY;
            }
        }

        return ZP_ERROR_OK;
    }

    ZP_Error getHealthMask(uint32_t& outPresent, uint32_t& outEnabled, uint32_t& outHealth) {
        outPresent = 0;
        outEnabled = 0;
        outHealth = 0;

        // Several BITs can share one sensor bit (IMU_INIT and IMU_DATA_VALID are both 3D_GYRO).
        // Collect failures separately so the result does not depend on table order.
        uint32_t failingMask = 0;

        for (uint16_t i = 0; i < BIT_TOTAL; i++) {
            const uint32_t SENSOR_BIT = BIT_CONFIG[i].mavSensorBit;
            if (SENSOR_BIT == 0 || bitStatus[i].live == BitState_e::UNKNOWN) {
                continue;
            }

            outPresent |= SENSOR_BIT;
            outEnabled |= SENSOR_BIT;

            if (bitStatus[i].live == BitState_e::SUCCESS) {
                outHealth |= SENSOR_BIT;
            } else {
                failingMask |= SENSOR_BIT;
            }
        }

        outHealth &= ~failingMask;

        ZP_BIT_ID blocking = ZP_BIT_ID::NUM_BIT_IDS;
        outPresent |= MAV_SYS_STATUS_PREARM_CHECK;
        outEnabled |= MAV_SYS_STATUS_PREARM_CHECK;
        if (prearmCheck(blocking) == ZP_ERROR_OK) {
            outHealth |= MAV_SYS_STATUS_PREARM_CHECK;
        }

        return ZP_ERROR_OK;
    }

    const char* name(ZP_BIT_ID id) {
        if (!indexValid(id)) {
            return "UNKNOWN_BIT";
        }

        return BIT_CONFIG[static_cast<uint16_t>(id)].name;
    }
}
