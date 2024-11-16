#pragma once

#include <gmock/gmock.h>
#include <iwdg_iface.hpp>

class MockWatchdog : public IndependentWatchdog{
    public:
        MOCK_METHOD(bool, refreshWatchdog, (), (override));
}