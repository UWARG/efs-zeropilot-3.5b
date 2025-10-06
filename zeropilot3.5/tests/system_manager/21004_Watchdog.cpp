#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "system_manager.hpp"
#include "iwdg_mock.hpp"
#include "logger_mock.hpp"
#include "rc_mock.hpp"
#include "queue_mock.hpp"
#include "tm_mock.hpp"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::StrictMock;

TEST(SMTest, 21004_Watchdog) 
{  
    StrictMock<MockWatchdog> mockIWDG;
    NiceMock<MockLogger> mockLogger;
    NiceMock<MockRCReceiver> mockRC;
    NiceMock<MockMessageQueue<RCMotorControlMessage_t>> mockAmQueue;
    NiceMock<MockTMQueue> mockTMQueue;
    NiceMock<MockMessageQueue<char[100]>> mockSmLoggerQueue;

    SystemManager sm(&mockIWDG, &mockLogger, &mockRC, &mockAmQueue, &mockTMQueue, &mockSmLoggerQueue);

    // Test: Verify watchdog is refreshed on every smUpdate() call (4 times total)
    EXPECT_CALL(mockIWDG, refreshWatchdog())
        .Times(4);

    // Call smUpdate 4 times - each should trigger refreshWatchdog()
    sm.smUpdate();
    sm.smUpdate();
    sm.smUpdate();
    sm.smUpdate();
}
