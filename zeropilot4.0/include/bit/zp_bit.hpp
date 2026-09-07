#pragma once

#include <cstdint>
#include "zp_error.h"
#include "systemutils_iface.hpp"

// Built-in test. Managers report the ZP_ERROR_e they already compute; this module debounces each
// check against a millisecond threshold, latches critical faults, and hands state transitions to
// handlers registered by System Manager.
//
// Threading: each BIT is written by exactly one manager (see the owner column in zp_bit.cpp).
// There is no lock; the single-writer rule is what makes that safe.

enum class BitPhase_e : uint8_t {
    POWER_ON,   // reported once during init, result latches for the boot
    CONTINUOUS  // reported every tick by its owning manager
};

enum class BitLevel_e : uint8_t {
    WARNING,   // reported to the GCS, no action
    CRITICAL   // reported, latched, and drives the configured failsafe action
};

enum class BitState_e : uint8_t {
    UNKNOWN,  // never reported
    PASSING,
    FAILING   // debounce threshold satisfied
};

enum class ZP_BIT_ID : uint16_t { // NOLINT
    // --- POWER_ON ---
    PARAM_TABLE_INIT,
    IMU_INIT,
    GPS_INIT,
    BARO_INIT,
    RC_INIT,
    PM_INIT,
    TELEM_INIT,
    RANGEFINDER_INIT,
    MOTOR_INIT,
    CAN_INIT,

    // --- CONTINUOUS ---
    RC_DATA_VALID,
    IMU_DATA_VALID,
    GPS_DATA_VALID,
    BARO_DATA_VALID,
    PM_DATA_VALID,
    RANGEFINDER_DATA_VALID,
    TELEM_LINK_VALID,
    BATT_LOW,
    BATT_CRITICAL,
    AM_LOOP_TIMING,
    SM_LOOP_TIMING,
    TM_LOOP_TIMING,

    BIT_COUNT
};

// Fired once per live-state transition, never per report
typedef void (*BitHandlerCb_t)(void* context, ZP_BIT_ID id, BitLevel_e level, BitState_e state);

namespace ZP_BIT {

    // Must be called once at boot before any report(). The clock supplies the millisecond
    // timebase the persistence thresholds are measured against.
    ZP_ERROR_e init(ISystemUtils* clock);

    // Reports one check. Returns status unchanged so a call site stays a single expression:
    //   result |= ZP_BIT::report(ZP_BIT_ID::IMU_DATA_VALID, imuDriver->readRawData(data));
    ZP_ERROR_e report(ZP_BIT_ID id, ZP_ERROR_e status);

    ZP_ERROR_e bindHandlerInternal(ZP_BIT_ID id, void* context, BitHandlerCb_t handler);

    template <typename T>
    ZP_ERROR_e bindHandler(ZP_BIT_ID id, T* context, void (*handler)(T*, ZP_BIT_ID, BitLevel_e, BitState_e)) {
        return bindHandlerInternal(id, static_cast<void*>(context), reinterpret_cast<BitHandlerCb_t>(handler));
    }

    // Fires the handler for every BIT whose live state changed since the last call.
    // Called once per tick by System Manager only.
    ZP_ERROR_e dispatch();

    // Clears every latched fault. Called by System Manager on the armed -> disarmed edge.
    ZP_ERROR_e clearLatched();

    // Overrides the table defaults. RC_DATA_VALID is seeded from the RC_FS_TIMEOUT param so the
    // migration off AM's old failsafe timer preserves its timing exactly.
    ZP_ERROR_e setPersistence(ZP_BIT_ID id, uint32_t failMs, uint32_t clearMs);

    ZP_ERROR_e getLive(ZP_BIT_ID id, BitState_e& outState);
    ZP_ERROR_e getLatched(ZP_BIT_ID id, BitState_e& outState);

    // ZP_ERROR_OK means armable. Otherwise outFirstBlocking names the offender.
    ZP_ERROR_e prearmCheck(ZP_BIT_ID& outFirstBlocking);

    // MAVLink SYS_STATUS onboard_control_sensors_* bitmasks
    ZP_ERROR_e getHealthMask(uint32_t& outPresent, uint32_t& outEnabled, uint32_t& outHealth);

    const char* name(ZP_BIT_ID id);
}
