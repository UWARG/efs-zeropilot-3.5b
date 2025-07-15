#include "system_manager.hpp"

SystemManager::SystemManager(
    IIndependentWatchdog *iwdgDriver,
    ILogger *loggerDriver,
    IRCReceiver *rcDriver, 
    IMessageQueue<RCMotorControlMessage_t> *amRCQueue, 
    IMessageQueue<char[100]> *smLoggerQueue) : 
        iwdgDriver(iwdgDriver),
        loggerDriver(loggerDriver),
        rcDriver(rcDriver), 
        amRCQueue(amRCQueue),
        smLoggerQueue(smLoggerQueue) {}

void SystemManager::runControlLoopIteration() {
    // Kick the watchdog
    iwdgDriver->refreshWatchdog();

    // Get RC data from the RC receiver and passthrough to AM if new
    static int oldDataCount = 0;
    static bool rcConnected = true;

    RCControl rcData = rcDriver->getRCData();
    if (rcData.isDataNew) {
        oldDataCount = 0;
        sendRCDataToAttitudeManager(rcData);

        if (!rcConnected) {
            loggerDriver->log("RC Reconnected");
            rcConnected = true;
        }
    } else {
        oldDataCount += 1;

        if ((oldDataCount * SM_CONTROL_LOOP_DELAY > SM_RC_TIMEOUT) && rcConnected) {
            loggerDriver->log("RC Disconnected");
            rcConnected = false;
        }
    }

    // Log if new messages
    if (smLoggerQueue->count() > 0) {
        sendMessagesToLogger();
    }
}

void SystemManager::sendRCDataToAttitudeManager(const RCControl &rcData) {
    RCMotorControlMessage_t rcDataMessage;

    rcDataMessage.roll = rcData.roll;
    rcDataMessage.pitch = rcData.pitch;
    rcDataMessage.yaw = rcData.yaw;
    rcDataMessage.throttle = rcData.throttle;
    rcDataMessage.arm = rcData.arm;
    rcDataMessage.flapAngle = rcData.aux2;

    amRCQueue->push(&rcDataMessage);
}

void SystemManager::sendMessagesToLogger() {
    static char messages[16][100];
    int msgCount = 0;

    while (smLoggerQueue->count() > 0) {
        smLoggerQueue->get(&messages[msgCount]);
        msgCount++;
    }

    loggerDriver->log(messages, msgCount);
}
