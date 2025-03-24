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
      uint16_t voltages[10] = {};
      int16_t current_battery = 0;
      int32_t current_consumed = 0;
      int32_t energy_consumed = 0;
      int8_t battery_remaining = 0;
      int32_t time_remaining = 0;
      uint8_t charge_state = 0;
      uint16_t voltages_ext[4] = {};
  } BMData_t;
} TMMessageData_t;

typedef struct{
    enum{
        GPOS_DATA,
        AM_DATA,
        BM_DATA
    } DataType;
    TMMessageData_t tm_message_data;
    int32_t time_boot_ms = 0;
} TMMessage_t;

//