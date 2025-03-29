#pragma once
#include <cstdint>

typedef union{
  struct{
      int32_t alt = 0;
      int32_t lat = 0;
      int32_t lon = 0;
      int32_t relative_alt = 0;
      int16_t vx = 0;
      int16_t vy = 0;
      int16_t vz = 0;
      uint16_t hdg = 0;
  } GPOSData_t;
  struct{
      float roll = 0;
      float pitch = 0;
      float yaw = 0;
      float rollspeed = 0;
      float pitchspeed = 0;
      float yawspeed = 0;
  } AMData_t;
  struct{
      int16_t temperature = 0;
      uint16_t voltages[3] = {};
      int16_t current_battery = 0;
      int32_t current_consumed = 0;
      int32_t energy_consumed = 0;
      int8_t battery_remaining = 0;
      int32_t time_remaining = 0;
      uint8_t charge_state = 0; // 1 = Normal, 2 = Low, 3 = Critical
  } BMData_t;
} TMMessageData_t;

typedef struct{
    enum{
        GPOS_DATA,
        AM_DATA,
        BM_DATA
    } DataType;
    TMMessageData_t tm_message_data;
    uint32_t time_boot_ms = 0;
} TMMessage_t;

inline TMMessage_t GPOSData_Pack(uint32_t time_boot_ms, int32_t alt, int32_t lat, int32_t lon, int32_t relative_alt, int16_t vx, int16_t vy, int16_t vz,uint16_t hdg) {
    const TMMessageData_t data = {.GPOSData_t={alt, lat, lon ,relative_alt, vx, vy, vz, hdg }};
    return TMMessage_t{TMMessage_t::GPOS_DATA, data, time_boot_ms};
}

inline TMMessage_t AMData_Pack(uint32_t time_boot_ms, float roll, float pitch, float yaw, float rollspeed, float pitchspeed, float yawspeed) {
    const TMMessageData_t data = {.AMData_t ={roll, pitch, yaw, rollspeed, pitchspeed, yawspeed }};
    return TMMessage_t{TMMessage_t::AM_DATA, data, time_boot_ms};
}

inline TMMessage_t BMData_Pack(uint32_t time_boot_ms, int16_t temperature, uint16_t *voltages, int16_t current_battery, int32_t current_consumed,
    int32_t energy_consumed, int8_t battery_remaining, int32_t time_remaining, uint8_t charge_state) {

    const TMMessageData_t data = {.BMData_t ={temperature, voltages[0], voltages[1], voltages[2], current_battery,
    current_consumed, energy_consumed, battery_remaining, time_remaining, charge_state}};
    return TMMessage_t{TMMessage_t::BM_DATA, data, time_boot_ms};
}
