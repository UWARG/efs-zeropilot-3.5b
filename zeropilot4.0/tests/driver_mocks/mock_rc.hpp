#pragma once 

#include "rc_iface.hpp"
#include <gmock/gmock.h>


class MockRCReceiver : public IRCReceiver{
    public:
       MOCK_METHOD(ZP_ERROR_e, getRCData, (RCControl &data), (override));
};
