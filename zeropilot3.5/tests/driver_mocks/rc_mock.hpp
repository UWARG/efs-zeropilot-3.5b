#pragma once 

#include "rc_iface.hpp"
#include <gmock/gmock.h>


class MockSBUSReceiver : public ISBUSReceiver{
    public:
       MOCK_METHOD(RCControl_t, getRCData, (), (override));
};