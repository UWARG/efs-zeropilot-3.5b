#pragma once

#include "../gps/gps_datatypes.hpp"

#include <cstdint>


class IGPS {
protected:
    IGPS() = default;

public:
    virtual ~IGPS() = default;

    virtual GpsData_t readData() = 0;
};
