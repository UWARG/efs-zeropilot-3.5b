#include <gtest/gtest.h>
#include "system_manager.hpp"
#include "iwdg_mock.hpp"
#include "logger_mock.hpp"
#include "rc_mock.hpp"
#include "queue_mock.hpp"
#include "tm_mock.hpp"

using ::testing::_;

TEST(SMTest, 21001_Default) 
{  
    MockWatchdog mockIWDG;
    MockLogger mockLogger;
    MockRCReceiver mockRC;
    MockMessageQueue<RCMotorControlMessage_t> mockAmQueue;
    MockTMQueue mockTMQueue;
    MockMessageQueue<char[100]> mockSmLoggerQueue;
    SystemManager sm(&mockIWDG, &mockLogger, &mockRC, &mockAmQueue, &mockTMQueue, &mockSmLoggerQueue);

    EXPECT_CALL(mockIWDG, refreshWatchdog());
    EXPECT_CALL(mockRC, getRCData());
    EXPECT_CALL(mockSmLoggerQueue, count());

    sm.smUpdate();
}