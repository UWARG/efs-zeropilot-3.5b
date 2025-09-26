#include "system_manager.hpp"

SystemManager::SystemManager(
    IIndependentWatchdog *iwdgDriver,
    ILogger *loggerDriver,
    IRCReceiver *rcDriver,
    IMessageQueue<RCMotorControlMessage_t> *amRCQueue,
    IMessageQueue<TMMessage_t> *tmQueue,
    IMessageQueue<char[100]> *smLoggerQueue) :
        iwdgDriver(iwdgDriver),
        loggerDriver(loggerDriver),
        rcDriver(rcDriver),
        amRCQueue(amRCQueue),
        tmQueue(tmQueue),
        smLoggerQueue(smLoggerQueue) {}

void SystemManager::smUpdate() {
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

        if ((oldDataCount * SM_MAIN_DELAY > 500) && rcConnected) {
            loggerDriver->log("RC Disconnected");
            rcConnected = false;
        }
    }

    // Send RC data to TM
    sendRCDataToTelemetryManager(rcData);

    // Log if new messages
    if (smLoggerQueue->count() > 0) {
        sendMessagesToLogger();
    }
}

void SystemManager::sendRCDataToTelemetryManager(const RCControl &rcData) {
    TMMessage_t rcDataMsg =  rcDataPack(0, rcData.roll, rcData.pitch, rcData.yaw, rcData.throttle, rcData.aux2, rcData.arm);
    tmQueue->push(&rcDataMsg);
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
    static char messages[MAX_MSG_COUNT][100];
    int msgIdx = 0;

    while (smLoggerQueue->count() > 0 && msgIdx < MAX_MSG_COUNT) {
        smLoggerQueue->get(&messages[msgIdx]);
        msgIdx++;
    }

    loggerDriver->log(messages, msgIdx);
}
