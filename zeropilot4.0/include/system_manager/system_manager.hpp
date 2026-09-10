#pragma once

#include "iwdg_iface.hpp"
#include "systemutils_iface.hpp"
#include "mavlink.h"
#include "logger_iface.hpp"
#include "rc_iface.hpp"
#include "rc_motor_control.hpp"
#include "iwdg_iface.hpp"
#include "safety_switch_iface.hpp"
#include "tm_queue.hpp"
#include "queue_iface.hpp"
#include "power_module_iface.hpp"
#include "sm_param_setup.hpp"
#include "zp_error.h"
#include "zp_bit.hpp"
#include "soc_estimation.hpp"

#define SM_SCHEDULING_RATE_HZ 20
#define SM_TELEMETRY_HEARTBEAT_RATE_HZ 1
#define SM_TELEMETRY_RC_DATA_RATE_HZ 5
#define SM_TELEMETRY_BATTERY_DATA_RATE_HZ 1
#define SM_TELEMETRY_SYS_STATUS_RATE_HZ 1

#define SM_UPDATE_LOOP_DELAY_MS (1000 / SM_SCHEDULING_RATE_HZ)

// RC Arm threshold
static constexpr float SM_RC_ARM_THRESHOLD = 50.0f;

// Flightmode Count
static constexpr uint8_t SM_FLIGHTMODE_COUNT = 6;
static constexpr uint8_t SM_RC_REVERSIBLE_COUNT = 4;

// Calculated using 1165, 1295, 1425, 1555, 1685, and 1815 us as nominal values
static constexpr float SM_FLIGHTMODE1_MAX = 23.0f; // (1165 + 1295) / 2 = 1230 -> scaled/offset to 23.0
static constexpr float SM_FLIGHTMODE2_MAX = 36.0f; // (1295 + 1425) / 2 = 1360 -> scaled/offset to 36.0
static constexpr float SM_FLIGHTMODE3_MAX = 49.0f; // (1425 + 1555) / 2 = 1490 -> scaled/offset to 49.0
static constexpr float SM_FLIGHTMODE4_MAX = 62.0f; // (1555 + 1685) / 2 = 1620 -> scaled/offset to 62.0
static constexpr float SM_FLIGHTMODE5_MAX = 75.0f; // (1685 + 1815) / 2 = 1750 -> scaled/offset to 75.0

// Safety switch constants
static constexpr uint32_t SM_SAFETY_SWITCH_HOLD_THRESHOLD_MS = 2000;
static constexpr uint32_t SM_SAFETY_SWITCH_BLINK_RATE_HZ = 2;
static constexpr uint32_t SM_SAFETY_SWITCH_PREARM_MSG_INTERVAL_S = 10; // Send safety switch prearm message every 10 seconds

typedef struct {
    ZP_BIT_ID id;
    const char* failText;
    BitHandlerCb_t action;
} SMBitHandler_t;

class SystemManager {
    friend class SMParamSetup;

    public:
        SystemManager(
            ISystemUtils *systemUtilsDriver,
            IIndependentWatchdog *iwdgDriver,
            ILogger *loggerDriver,
            ISafetySwitch *safetySwitchDriver,
            IRCReceiver *rcDriver,
            IPowerModule *pmDriver,
            IMessageQueue<RCMotorControlMessage_t> *amRCQueue,
            IMessageQueue<TMMessage_t> *tmQueue,
            IMessageQueue<char[100]> *smLoggerQueue
        );

        void smUpdate(); // This function is the main function of SM, it should be called in the main loop of the system.

    private:
        ISystemUtils *systemUtilsDriver; // System utilities instance

        IIndependentWatchdog *iwdgDriver; // Independent Watchdog driver
        ILogger *loggerDriver; // Logger driver
        ISafetySwitch *safetySwitchDriver; // Safety switch driver
        IRCReceiver *rcDriver; // RC receiver driver
        IPowerModule *pmDriver; // Power module driver
        
        IMessageQueue<RCMotorControlMessage_t> *amRCQueue; // Queue driver for tx communication to the Attitude Manager
        IMessageQueue<TMMessage_t> *tmQueue; // Queue driver for tx communication to the Telemetry Manager
        IMessageQueue<char[100]> *smLoggerQueue; // Queue driver for rx communication from other modules to the System Manager for logging

        uint8_t smSchedulingCounter;

        FlightMode_e flightModes[SM_FLIGHTMODE_COUNT];

        bool isSafetySwitchEngaged;         // Flag to indicate if the safety switch is engaged
        uint32_t safetySwitchHoldCounterMs; // Counter to track how long the safety switch has been held
        bool safetySwitchTriggered;         // Flag to prevent toggling multiple times during a single long press
        uint32_t safetySwitchPrearmCntrMs;  // Counter to track time since last prearm message was sent
        
        // Reports the failure and takes no action
        static void reportBitCallback(void* context, ZP_BIT_ID id, BitLevel_e level, BitState_e state);

        // Reports, then blocks arming until disarmmed
        static void disarmBitCallback(void* context, ZP_BIT_ID id, BitLevel_e level, BitState_e state);

        static const SMBitHandler_t BIT_HANDLERS[static_cast<uint16_t>(ZP_BIT_ID::NUM_BIT_IDS)];

        ZP_Error bindBitHandlers();
        static const SMBitHandler_t& bitRow(ZP_BIT_ID id);
        ZP_Error reportLoopTiming(ZP_BIT_ID id, uint32_t maxExecUs, uint32_t budgetMs);

        ZP_Error safetySwitchUpdate();

        bool rcConnected;
        bool prevArmed;
        bool bitDisarmLatch; // Set by a BIT disarm action, cleared with the latched faults
        uint32_t bitPrearmCntrMs;

        bool rcChannelReversed[SM_RC_REVERSIBLE_COUNT];
        
        BatteryData_t batteryData;
        ZP_Error updateBatteryFSM();
        SocEstimator socEstimator;

        ZP_Error sendRCDataToAttitudeManager(const RCControl &rcData);
        ZP_Error sendRCDataToTelemetryManager(const RCControl &rcData);
        ZP_Error sendHeartbeatDataToTelemetryManager(uint8_t baseMode, uint32_t customMode, MAV_STATE systemStatus);
        ZP_Error sendSysStatusToTelemetryManager();
        ZP_Error sendBatteryDataToTelemetryManager(const BatteryData_t &batteryData, const uint8_t batteryId);
        ZP_Error sendStatusTextToTelemetryManager(MAV_SEVERITY severity, const char text[50], uint16_t id = 0, uint8_t chunk_seq = 0);

        ZP_Error decodeRawFlightMode(float flightModeRawValue, FlightMode_e& flightMode);

        ZP_Error sendMessagesToLogger();

        uint8_t profilerId;

        SMParamSetup paramSetup;

        uint8_t profilerBuf[256];
        TaskProfile profiles[MAX_PROFILED_TASKS];
};

inline const SMBitHandler_t SystemManager::BIT_HANDLERS[static_cast<uint16_t>(ZP_BIT_ID::NUM_BIT_IDS)] = {
    {ZP_BIT_ID::PARAM_TABLE_INIT,       "PreArm: Param table init failed", reportBitCallback},
    {ZP_BIT_ID::IMU_INIT,               "PreArm: IMU init failed",         reportBitCallback},
    {ZP_BIT_ID::GPS1_INIT,              "GPS1 init failed",                reportBitCallback},
    {ZP_BIT_ID::GPS2_INIT,              "GPS2 init failed",                reportBitCallback},
    {ZP_BIT_ID::BARO_INIT,              "Baro init failed",                reportBitCallback},
    {ZP_BIT_ID::RC_INIT,                "PreArm: RC init failed",          reportBitCallback},
    {ZP_BIT_ID::PM_INIT,                "Power module init failed",        reportBitCallback},
    {ZP_BIT_ID::TELEM_INIT,             "Telemetry init failed",           reportBitCallback},
    {ZP_BIT_ID::RANGEFINDER_INIT,       "Rangefinder init failed",         reportBitCallback},
    {ZP_BIT_ID::MOTOR_INIT,             "PreArm: Motor init failed",       reportBitCallback},
    {ZP_BIT_ID::CAN_INIT,               "CAN init failed",                 reportBitCallback},

    {ZP_BIT_ID::RC_DATA_VALID,          "PreArm: RC disconnected",         disarmBitCallback},
    {ZP_BIT_ID::IMU_DATA_VALID,         "PreArm: IMU data invalid",        reportBitCallback},
    {ZP_BIT_ID::GPS_DATA_VALID,         "GPS data invalid",                reportBitCallback},
    {ZP_BIT_ID::BARO_DATA_VALID,        "Baro data invalid",               reportBitCallback},
    {ZP_BIT_ID::PM_DATA_VALID,          "Power module data invalid",       reportBitCallback},
    {ZP_BIT_ID::RANGEFINDER_DATA_VALID, "Rangefinder data invalid",        reportBitCallback},
    {ZP_BIT_ID::TELEM_LINK_VALID,       "Telemetry link lost",             reportBitCallback},
    {ZP_BIT_ID::BATT_LOW,               "Battery low",                     reportBitCallback},
    {ZP_BIT_ID::BATT_CRITICAL,          "PreArm: Battery critical",        reportBitCallback},
    {ZP_BIT_ID::AM_LOOP_TIMING,         "AM loop overrun",                 reportBitCallback},
    {ZP_BIT_ID::SM_LOOP_TIMING,         "SM loop overrun",                 reportBitCallback},
    {ZP_BIT_ID::TM_LOOP_TIMING,         "TM loop overrun",                 reportBitCallback},
};
