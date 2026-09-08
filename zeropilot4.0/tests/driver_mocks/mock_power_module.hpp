#pragma once

#include <gmock/gmock.h>
#include "power_module_iface.hpp"

class MockPowerModule : public IPowerModule {
public:
    MOCK_METHOD(ZP_ERROR_e, readData, (PMData_t *data), (override));
};
