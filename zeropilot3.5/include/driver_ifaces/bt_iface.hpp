#pragma once

#include <cstdint>
#include "battery_datatypes.hpp"

class IBatteryReceiver {
    protected: 
        IBatteryReceiver() = default;
        
    public:
        virtual ~IBatteryReceiver() = default;
        
        // Get battery data that is parsed from the battery monitoring system
        virtual BatteryData getBatteryData() = 0;
};
