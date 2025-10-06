#include <gtest/gtest.h>
#include "system_manager.hpp"
#include "iwdg_mock.hpp"
#include "logger_mock.hpp"
#include "rc_mock.hpp"
#include "queue_mock.hpp"
#include "tm_mock.hpp"

using ::testing::_;
using ::testing::NiceMock;

TEST(SMTest, 21002_Logger) 
{  
    NiceMock<MockWatchdog> mockIWDG;
    NiceMock<MockLogger> mockLogger;
    NiceMock<MockRCReceiver> mockRC;
    NiceMock<MockMessageQueue<RCMotorControlMessage_t>> mockAmQueue;
    NiceMock<MockTMQueue> mockTMQueue;
    NiceMock<MockMessageQueue<char[100]>> mockSmLoggerQueue;

    SystemManager sm(&mockIWDG, &mockLogger, &mockRC, &mockAmQueue, &mockTMQueue, &mockSmLoggerQueue);

    mockSmLoggerQueue.delegateToFake();

    char msg[100] = {};
    
    for (int i = 0; i < 17; i++) {
        snprintf(msg, 100, "test %d", i);
        mockSmLoggerQueue.push(&msg);
    }

    EXPECT_EQ(mockSmLoggerQueue.count(), 17);
    EXPECT_CALL(mockLogger, log(_, 16));

    sm.smUpdate();

    EXPECT_CALL(mockLogger, log(_, 1));
    sm.smUpdate();
}