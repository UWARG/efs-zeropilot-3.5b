#include "zp_bit.hpp"
#include "mavlink.h"

namespace ZP_BIT {

    namespace {

        typedef struct {
            BitState_e live; // Current state
            BitState_e latched; // Latched on fault
            BitState_e runState; // Current running state after failMs or clearMs
            uint32_t edgeMs; // Timestamp of last change of running state
            uint32_t failMs;
            uint32_t clearMs;
            bool changed; // Flag to detect change
            uint32_t lastErrorBits; // Last reported ZP_Error, held raw since ZP_Error cannot be assigned
            void* context; // Context pointer for callback
            BitHandlerCb_t onChange; // Callback for on change
        } BitStatus_t;

        BitStatus_t bitStatus[static_cast<uint16_t>(ZP_BIT_ID::NUM_BIT_IDS)];
        ISystemUtils* clockDriver = nullptr;

        inline bool indexValid(ZP_BIT_ID id) {
            return id < ZP_BIT_ID::NUM_BIT_IDS;
        }
    }

    ZP_Error init(ISystemUtils* clock) {
        if (clock == nullptr) {
            return ZP_ERROR_NULLPTR;
        }

        clockDriver = clock;

        for (uint16_t i = 0; i < static_cast<uint16_t>(ZP_BIT_ID::NUM_BIT_IDS); i++) {
            bitStatus[i].live = BitState_e::UNKNOWN;
            bitStatus[i].latched = BitState_e::UNKNOWN;
            bitStatus[i].runState = BitState_e::UNKNOWN;
            bitStatus[i].edgeMs = 0;
            bitStatus[i].failMs = BIT_CONFIG[i].failMs;
            bitStatus[i].clearMs = BIT_CONFIG[i].clearMs;
            bitStatus[i].changed = false;
            bitStatus[i].lastErrorBits = ZP_ERROR_OK.raw();
            bitStatus[i].context = nullptr;
            bitStatus[i].onChange = nullptr;
        }

        return ZP_ERROR_OK;
    }

    ZP_Error report(ZP_BIT_ID id, ZP_Error status) {
        if (!indexValid(id)) {
            return ZP_ERROR_RANGE;
        }
        if (clockDriver == nullptr) {
            return ZP_ERROR_NOT_READY;
        }

        const BitConfig_t& config = BIT_CONFIG[static_cast<uint16_t>(id)];
        BitStatus_t& state = bitStatus[static_cast<uint16_t>(id)];

        const BitState_e OBSERVED = (status == ZP_ERROR_OK) ? BitState_e::SUCCESS : BitState_e::FAILURE;
        const uint32_t NOW = clockDriver->getCurrentTimestampMs();

        // Kept so getError can say why the BIT is failing. A passing report clears it.
        state.lastErrorBits = status.raw();

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

        return ZP_ERROR_OK;
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
        for (uint16_t i = 0; i < static_cast<uint16_t>(ZP_BIT_ID::NUM_BIT_IDS); i++) {
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
        for (uint16_t i = 0; i < static_cast<uint16_t>(ZP_BIT_ID::NUM_BIT_IDS); i++) {
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

    ZP_Error getError(ZP_BIT_ID id) {
        if (!indexValid(id)) {
            return ZP_ERROR_RANGE;
        }
        return ZP_Error(bitStatus[static_cast<uint16_t>(id)].lastErrorBits);
    }

    ZP_Error prearmCheck(ZP_BIT_ID& outFirstBlocking) {
        for (uint16_t i = 0; i < static_cast<uint16_t>(ZP_BIT_ID::NUM_BIT_IDS); i++) {
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

        uint32_t failingMask = 0;

        for (uint16_t i = 0; i < static_cast<uint16_t>(ZP_BIT_ID::NUM_BIT_IDS); i++) {
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
