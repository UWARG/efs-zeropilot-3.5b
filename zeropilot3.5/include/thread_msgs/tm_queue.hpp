#pragma once
#include <cstdint>

typedef union TMMessageData_u {
  struct{
      uint8_t baseMode;
      uint32_t customMode;
      uint8_t systemStatus;
  } heartbeatData;
  struct{
      int32_t alt;
      int32_t lat;
      int32_t lon;
      int32_t relativeAlt;
      int16_t vx;
      int16_t vy;
      int16_t vz;
      uint16_t hdg;
  } gposData;
  struct{
      uint16_t roll;
      uint16_t pitch;
      uint16_t yaw;
      uint16_t throttle;
      uint16_t flapAngle;
      uint16_t arm;
  } rcData;
  struct{
      int16_t temperature;
      uint16_t* voltages;
      int16_t currentBattery;
      int32_t currentConsumed;
      int32_t energyConsumed;
      int8_t batteryRemaining;
      int32_t timeRemaining;
      uint8_t chargeState; // 1 = Normal, 2 = Low, 3 = Critical
  } bmData;
} TMMessageData_t;

typedef struct TMMessage{
    enum{
        HEARTBEAT_DATA,
        GPOS_DATA,
        RC_DATA,
        BM_DATA
    } dataType;
    TMMessageData_t tmMessageData;
    uint32_t timeBootMs = 0;
} TMMessage_t;

inline TMMessage_t heartbeatPack(const uint32_t TIME_BOOT_MS, const uint8_t BASE_MODE, const uint32_t CUSTOM_MODE, const uint8_t SYSTEM_STATUS) {
    const TMMessageData_t DATA = {.heartbeatData={BASE_MODE, CUSTOM_MODE, SYSTEM_STATUS }};
    return TMMessage_t{TMMessage_t::HEARTBEAT_DATA, DATA, TIME_BOOT_MS};
}

inline TMMessage_t gposDataPack(const uint32_t TIME_BOOT_MS, const int32_t ALT, const int32_t LAT, const int32_t LON, const int32_t RELATIVE_ALT, const int16_t VX, const int16_t VY, const int16_t VZ, const uint16_t HDG) {
    const TMMessageData_t DATA = {.gposData={ALT, LAT, LON, RELATIVE_ALT, VX, VY, VZ, HDG }};
    return TMMessage_t{TMMessage_t::GPOS_DATA, DATA, TIME_BOOT_MS};
}

inline TMMessage_t rcDataPack(const uint32_t TIME_BOOT_MS, const float ROLL, const float PITCH, const float YAW, const float THROTTLE, const float FLAP_ANGLE, const float ARM) {
    const auto ROLL_PPM = static_cast<uint16_t>(1000 + ROLL * 10);
    const auto PITCH_PPM = static_cast<uint16_t>(1000 + PITCH * 10);
    const auto YAW_PPM = static_cast<uint16_t>(1000 + YAW * 10);
    const auto THROTTLE_PPM = static_cast<uint16_t>(1000 + THROTTLE * 10);
    const auto FLAP_ANGLE_PPM = static_cast<uint16_t>(1000 + FLAP_ANGLE * 10);
    const auto ARM_PPM = static_cast<uint16_t>(1000 + ARM * 10);
    const TMMessageData_t DATA = {.rcData ={ROLL_PPM, PITCH_PPM, YAW_PPM, THROTTLE_PPM, FLAP_ANGLE_PPM, ARM_PPM }};
    return TMMessage_t{TMMessage_t::RC_DATA, DATA, TIME_BOOT_MS};
}

inline TMMessage_t bmDataPack(const uint32_t TIME_BOOT_MS, int16_t temperature, const float *const VOLTAGES, const uint8_t VOLTAGE_LEN, const int16_t CURRENT_BATTERY, const int32_t CURRENT_CONSUMED,
    const int32_t ENERGY_CONSUMED, const int8_t BATTERY_REMAINING, const int32_t TIME_REMAINING, const uint8_t CHARGE_STATE) {
    uint16_t mavlinkVoltageArray[16] = {UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX};
    for (int i = 0; i < VOLTAGE_LEN; i++) {
    	mavlinkVoltageArray[i] = static_cast<uint16_t>(VOLTAGES[i]);
    }
    if (temperature == -1) {
        temperature = INT16_MAX;
    }
    const TMMessageData_t DATA = {.bmData ={temperature, mavlinkVoltageArray, CURRENT_BATTERY,
    CURRENT_CONSUMED, ENERGY_CONSUMED, BATTERY_REMAINING, TIME_REMAINING, CHARGE_STATE}};
    return TMMessage_t{TMMessage_t::BM_DATA, DATA, TIME_BOOT_MS};
}
