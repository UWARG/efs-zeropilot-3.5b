#pragma once

#include <gmock/gmock.h>
#include "iwdg_iface.hpp"

class MockWatchdog : public IIndependentWatchdog {
    public:
        MOCK_METHOD(ZP_ERROR_e, refreshWatchdog, (), (override));
};
