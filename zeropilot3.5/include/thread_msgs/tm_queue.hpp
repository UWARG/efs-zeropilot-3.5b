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

inline TMMessage_t heartbeatPack(const uint32_t time_boot_ms, const uint8_t base_mode, const uint32_t custom_mode, const uint8_t system_status) {
    const TMMessageData_t DATA = {.heartbeatData={base_mode, custom_mode, system_status }};
    return TMMessage_t{TMMessage_t::HEARTBEAT_DATA, DATA, time_boot_ms};
}

inline TMMessage_t gposDataPack(const uint32_t time_boot_ms, const int32_t alt, const int32_t lat, const int32_t lon, const int32_t relative_alt, const int16_t vx, const int16_t vy, const int16_t vz, const uint16_t hdg) {
    const TMMessageData_t DATA = {.gposData={alt, lat, lon, relative_alt, vx, vy, vz, hdg }};
    return TMMessage_t{TMMessage_t::GPOS_DATA, DATA, time_boot_ms};
}

inline TMMessage_t rcDataPack(const uint32_t time_boot_ms, const float roll, const float pitch, const float yaw, const float throttle, const float flap_angle, const float arm) {
    const auto rollPPM = static_cast<uint16_t>(1000 + roll * 10);
    const auto pitchPPM = static_cast<uint16_t>(1000 + pitch * 10);
    const auto yawPPM = static_cast<uint16_t>(1000 + yaw * 10);
    const auto throttlePPM = static_cast<uint16_t>(1000 + throttle * 10);
    const auto flapAnglePPM = static_cast<uint16_t>(1000 + flap_angle * 10);
    const auto armPPM = static_cast<uint16_t>(1000 + arm * 10);
    const TMMessageData_t DATA = {.rcData ={rollPPM, pitchPPM, yawPPM, throttlePPM, flapAnglePPM, armPPM }};
    return TMMessage_t{TMMessage_t::RC_DATA, DATA, time_boot_ms};
}

inline TMMessage_t bmDataPack(const uint32_t time_boot_ms, int16_t temperature, const float *voltages, const uint8_t voltage_len, const int16_t current_battery, const int32_t current_consumed,
    const int32_t energy_consumed, const int8_t battery_remaining, const int32_t time_remaining, const uint8_t charge_state) {
    uint16_t mavlinkVoltageArray[16] = {UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX};
    for (int i = 0; i < voltage_len; i++) {
    	mavlinkVoltageArray[i] = static_cast<uint16_t>(voltages[i]);
    }
    if (temperature == -1) {
        temperature = INT16_MAX;
    }
    const TMMessageData_t DATA = {.bmData ={temperature, mavlinkVoltageArray, current_battery,
    current_consumed, energy_consumed, battery_remaining, time_remaining, charge_state}};
    return TMMessage_t{TMMessage_t::BM_DATA, DATA, time_boot_ms};
}
