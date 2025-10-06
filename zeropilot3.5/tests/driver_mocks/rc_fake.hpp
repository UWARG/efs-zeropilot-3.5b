#pragma once 

#include "rc_iface.hpp"
#include <gmock/gmock.h>


class FakeRCReceiver : public IRCReceiver{
    private:
        RCControl rcData;
    public:
        RCControl getRCData() override {
            rcData.isDataNew = true;
            return rcData;
        }
};
