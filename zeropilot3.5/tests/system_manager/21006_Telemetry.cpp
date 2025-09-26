#include <gtest/gtest.h>
#include "system_manager.hpp"
#include "iwdg_mock.hpp"
#include "logger_mock.hpp"
#include "rc_mock.hpp"
#include "queue_mock.hpp"
#include "tm_mock.hpp"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Invoke;


// Test that RC data flows to telemetry manager
TEST(SMTest, 21005_GetRCDataFlowsToTelemetryManager) 
{
    NiceMock<MockWatchdog> mockIWDG;
    NiceMock<MockLogger> mockLogger;
    MockRCReceiver mockRC;
    NiceMock<MockMessageQueue<RCMotorControlMessage_t>> mockAmQueue; 
    MockTMQueue mockTMQueue;  // Not using NiceMock so we can verify strict expectations
    NiceMock<MockMessageQueue<char[100]>> mockSmLoggerQueue;

    SystemManager sm(&mockIWDG, &mockLogger, &mockRC, &mockAmQueue, &mockTMQueue, &mockSmLoggerQueue);

    // Create test RC data
    RCControl testData;
    testData.isDataNew = true;
    testData.roll = 25.0f;
    testData.pitch = 35.0f;
    testData.yaw = 45.0f;
    testData.throttle = 55.0f;
    testData.arm = 1.0f;
    testData.aux2 = 65.0f;

    // Verify getRCData() is called and returns our test data
    EXPECT_CALL(mockRC, getRCData())
        .Times(1)
        .WillOnce(::testing::Return(testData));

    EXPECT_CALL(mockTMQueue, push)
        .WillOnce(::testing::Invoke([](TMMessage_t* msg) -> int {
            // Verify the message type is RC_DATA
            EXPECT_EQ(msg->dataType, TMMessage_t::RC_DATA);
            
            // Verify the data was correctly converted to PPM format
            // PPM format is 1000 + (value * 10)
            EXPECT_EQ(msg->tmMessageData.rcData.roll, 1250);     // 1000 + 25*10
            EXPECT_EQ(msg->tmMessageData.rcData.pitch, 1350);    // 1000 + 35*10
            EXPECT_EQ(msg->tmMessageData.rcData.yaw, 1450);      // 1000 + 45*10
            EXPECT_EQ(msg->tmMessageData.rcData.throttle, 1550); // 1000 + 55*10
            EXPECT_EQ(msg->tmMessageData.rcData.arm, 1010);      // 1000 + 1*10
            EXPECT_EQ(msg->tmMessageData.rcData.flapAngle, 1650); // 1000 + 65*10
            return 0;
        }));

    sm.smUpdate();
}