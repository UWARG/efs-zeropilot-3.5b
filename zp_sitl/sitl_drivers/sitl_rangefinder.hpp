#pragma once

#include "rangefinder_iface.hpp"

class SITL_Rangefinder : public IRangefinder {
private: 
    RangefinderData_t data = {};
public:

    ZP_Error init() override { return ZP_ERROR_OK; }
    RangefinderData_t readData() override { return data; }

    void update_from_plant(float sim_altitude) {
        data.distance = sim_altitude;
        data.signalStrength = 65535;
        data.isValid = true;
        data.isNew = true;
    }
};
