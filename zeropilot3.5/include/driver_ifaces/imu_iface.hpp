#pragma once

#include <cstdint>

#include "imu_datatypes.hpp"

class IIMU {
protected:
	IIMU() = default;

public:
    virtual ~IIMU() = default;
	virtual int init() = 0;
	virtual RAW_IMU_t readRawData() = 0;
	virtual SCALED_IMU_t scaleIMUData(const RAW_IMU_t &rawData) = 0;
};
