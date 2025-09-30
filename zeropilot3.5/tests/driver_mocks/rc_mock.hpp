#pragma once 

#include <gmock/gmock.h>
#include "rc_fake.hpp"

class MockRCReceiver : public IRCReceiver{
    private:
        FakeRCReceiver fakeRC;
    public:
        MOCK_METHOD(RCControl, getRCData, (), (override));

        void delegateToFake() {
            ON_CALL(*this, getRCData).WillByDefault([this] () {
                return fakeRC.getRCData();
            });
        }
};
