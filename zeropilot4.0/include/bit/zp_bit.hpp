#pragma once

#include <cstdint>
#include "zp_error.h"
#include "systemutils_iface.hpp"

enum class BitPhase_e : uint8_t {
    POWER_ON,
    CONTINUOUS
};

enum class BitLevel_e : uint8_t {
    WARNING,
    CRITICAL
};

enum class BitState_e : uint8_t {
    UNKNOWN,
    SUCCESS,
    FAILURE
};

enum class ZP_BIT_ID : uint16_t { // NOLINT
    //  POWER_ON
    PARAM_TABLE_INIT,
    IMU_INIT,
    GPS1_INIT,
    GPS2_INIT,
    BARO_INIT,
    RC_INIT,
    PM_INIT,
    TELEM_INIT,
    RANGEFINDER_INIT,
    MOTOR_INIT,
    CAN_INIT,

    // CONTINUOUS
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

    NUM_BIT_IDS
};

typedef void (*BitHandlerCb_t)(void* context, ZP_BIT_ID id, BitLevel_e level, BitState_e state);

namespace ZP_BIT {

    ZP_Error init(ISystemUtils* clock);

    // Reports the status of one BIT
    ZP_Error report(ZP_BIT_ID id, ZP_Error status);

    /*
    @brief Registers a handler for a BIT when the status changes
    @param context: pointer to a context object that can be accessed in the handler
    */
    ZP_Error bindHandler(ZP_BIT_ID id, void* context, BitHandlerCb_t handler);

    // Fires the handlers for each BIT which had a live state change
    ZP_Error dispatch();

    // Clears every latched fault
    ZP_Error clearLatched();

    /*
    @brief Sets the persistence value for a BIT for a state change
    @param failMs: The time to declare a BIT to be failed
    @param clearMs: The time to declare a BIT to be successful
    */
    ZP_Error setPersistence(ZP_BIT_ID id, uint32_t failMs, uint32_t clearMs);

    // Get live state
    ZP_Error getLive(ZP_BIT_ID id, BitState_e& outState);
    
    // Get latched state
    ZP_Error getLatched(ZP_BIT_ID id, BitState_e& outState);

    // ZP_ERROR_OK means armable. Otherwise outFirstBlocking is the first BIT that blocks arming
    ZP_Error prearmCheck(ZP_BIT_ID& outFirstBlocking);

    // MAVLink SYS_STATUS onboard_control_sensors_* bitmasks
    ZP_Error getHealthMask(uint32_t& outPresent, uint32_t& outEnabled, uint32_t& outHealth);

    const char* name(ZP_BIT_ID id);
}
