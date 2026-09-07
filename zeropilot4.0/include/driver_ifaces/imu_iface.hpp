#pragma once

#include <cstdint>
#include "imu_datatypes.hpp"
#include "zp_error.h"

class IIMU {
protected:
	IIMU() = default;

public:
    virtual ~IIMU() = default;
	virtual int init() = 0;
	virtual ZP_ERROR_e readRawData(RawImuBatch_t &rawDataBatch) = 0;
	virtual ZP_ERROR_e scaleIMUData(const RawImuBatch_t &rawDataBatch, ScaledImuBatch_t &scaledDataBatch) = 0;
	virtual float getODRHz() = 0;
	virtual GyroBias_t getGyroStartupBias(uint8_t imuId) = 0;
};
