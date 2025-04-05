#include <gtest/gtest.h>
#include "system_manager.hpp"
#include "logger_mock.hpp"
#include "rc_mock.hpp"
#include "queue_mock.hpp"

using ::testing::_;

TEST(SMTest, 21001_Watchdog) 
{  
    MockLogger mockLogger;
    MockRCReceiver mockRC;
    MockMessageQueue<RCMotorControlMessage_t> mockAmQueue;
    MockMessageQueue<char[100]> mockSmLoggerQueue;
    SystemManager sm(&mockLogger, &mockRC, &mockAmQueue, &mockSmLoggerQueue);

    EXPECT_CALL(mockRC, getRCData());

    sm.SMUpdate();
}