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

TEST(SMTest, 21005_RCData) 
{  
    NiceMock<MockWatchdog> mockIWDG;
    NiceMock<MockLogger> mockLogger;
    MockRCReceiver mockRC;  // Not using NiceMock here so we can verify strict call expectations
    NiceMock<MockMessageQueue<RCMotorControlMessage_t>> mockAmQueue;
    NiceMock<MockTMQueue> mockTMQueue;
    NiceMock<MockMessageQueue<char[100]>> mockSmLoggerQueue;

    SystemManager sm(&mockIWDG, &mockLogger, &mockRC, &mockAmQueue, &mockTMQueue, &mockSmLoggerQueue);

    mockRC.delegateToFake();

    // Expect getRCData() to be called at least once
    EXPECT_CALL(mockRC, getRCData());

    // Execute
    sm.smUpdate();

}

// Test that getRCData() is called and data flows through SystemManager
TEST(SMTest, 21005_GetRCDataFlowsToQueue) 
{
    NiceMock<MockWatchdog> mockIWDG;
    NiceMock<MockLogger> mockLogger;
    MockRCReceiver mockRC;
    MockMessageQueue<RCMotorControlMessage_t> mockAmQueue; 
    NiceMock<MockTMQueue> mockTMQueue;
    NiceMock<MockMessageQueue<char[100]>> mockSmLoggerQueue;

    SystemManager sm(&mockIWDG, &mockLogger, &mockRC, &mockAmQueue, &mockTMQueue, &mockSmLoggerQueue);

    // Create test RC data
    RCControl testData;
    testData.isDataNew = true;
    testData.roll = 10.0f;
    testData.pitch = 20.0f;
    testData.yaw = 30.0f;
    testData.throttle = 40.0f;
    testData.arm = 1.0f;
    testData.aux2 = 15.0f;

    // Verify getRCData() is called and returns our test data
    EXPECT_CALL(mockRC, getRCData())
        .Times(1)
        .WillOnce(::testing::Return(testData));

    // Verify that the data flows to the attitude manager queue
    EXPECT_CALL(mockAmQueue, push)
        .WillOnce(::testing::Invoke([](RCMotorControlMessage_t* msg) -> int {
            // Verify the data was correctly transferred
            EXPECT_EQ(msg->roll, 10.0f);
            EXPECT_EQ(msg->pitch, 20.0f);
            EXPECT_EQ(msg->yaw, 30.0f);
            EXPECT_EQ(msg->throttle, 40.0f);
            EXPECT_EQ(msg->arm, 1.0f);
            EXPECT_EQ(msg->flapAngle, 15.0f);
            return 0;
        }));

    // Execute the system update
    sm.smUpdate();
}
