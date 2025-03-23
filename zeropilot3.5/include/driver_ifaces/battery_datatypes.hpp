#pragma once


struct BatteryData {
    float voltage;      // Current battery voltage in volts
    float capacity;     // Current battery capacity in mAh
    bool isDataNew;     // Flag to indicate if data is new
};
