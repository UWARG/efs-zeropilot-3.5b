#pragma once

#include <gmock/gmock.h>
#include "imu_iface.hpp"

class MockIMU : public IIMU {
public:
    MOCK_METHOD(int, init, (), (override));
    MOCK_METHOD(ZP_ERROR_e, readRawData, (RawImuBatch_t &rawDataBatch), (override));
    MOCK_METHOD(ZP_ERROR_e, scaleIMUData, (const RawImuBatch_t &rawDataBatch, ScaledImuBatch_t &scaledDataBatch), (override));
    MOCK_METHOD(float, getODRHz, (), (override));
    MOCK_METHOD(GyroBias_t, getGyroStartupBias, (uint8_t imuId), (override));
};
