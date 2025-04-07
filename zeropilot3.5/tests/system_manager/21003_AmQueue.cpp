#include <gtest/gtest.h>
#include "system_manager.hpp"
#include "iwdg_mock.hpp"
#include "logger_mock.hpp"
#include "rc_mock.hpp"
#include "queue_mock.hpp"

using ::testing::_;
using ::testing::NiceMock;

TEST(SMTest, 21003_AmQueue) 
{  
    NiceMock<MockWatchdog> mockIWDG;
    NiceMock<MockLogger> mockLogger;
    NiceMock<MockRCReceiver> mockRC;
    MockMessageQueue<RCMotorControlMessage_t> mockAmQueue;
    NiceMock<MockMessageQueue<char[100]>> mockSmLoggerQueue;

    SystemManager sm(&mockIWDG, &mockLogger, &mockRC, &mockAmQueue, &mockSmLoggerQueue);

    mockRC.delegateToFake();
    mockAmQueue.delegateToFake();

    EXPECT_CALL(mockAmQueue, push);

    sm.SMUpdate();

    EXPECT_CALL(mockAmQueue, count());
    EXPECT_EQ(mockAmQueue.count(), 1);
}