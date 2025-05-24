#pragma once


struct BatteryData {
    float voltage;      // Current battery voltage in volts
    float capacity;     // Current battery capacity in mAh
    bool isDataNew;     // Flag to indicate if data is new

    BatteryData& operator=(const BatteryData& other) {
        this->isDataNew = other.isDataNew;
        return *this;
    }
};
