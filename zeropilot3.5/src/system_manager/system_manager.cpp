#include "system_manager.hpp"

SystemManager::SystemManager(
    IIndependentWatchdog *iwdgDriver,
    IRCReceiver *rcDriver, 
    IMessageQueue<RCMotorControlMessage_t> *amRCQueue, 
    IMessageQueue<char[100]> *smLoggerQueue,
    Logger *logger) :
        iwdgDriver_(iwdgDriver),
        rcDriver_(rcDriver), 
        amRCQueue_(amRCQueue),
        smLoggerQueue_(smLoggerQueue),
        logger(logger) {}

void SystemManager::SMUpdate() {
    // Kick the watchdog
    iwdgDriver_->refreshWatchdog();

    // Get RC data from the RC receiver and passthrough to AM if new
    static int oldDataCount = 0;
    static bool rcConnected = true;

    RCControl rcData = rcDriver_->getRCData();
    if (rcData.isDataNew) {
        oldDataCount = 0;
        sendRCDataToAttitudeManager(rcData);

        if (!rcConnected) {
            logger->log("RC Reconnected");
            rcConnected = true;
        }
    } else {
        oldDataCount += 1;

        if ((oldDataCount * SM_MAIN_DELAY > 500) && rcConnected) {
            logger->log("RC Disconnected");
            rcConnected = false;
        }
    }

    // Log if new messages
    if (smLoggerQueue_->count() > 0) {
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

    amRCQueue_->push(&rcDataMessage);
}

void SystemManager::sendMessagesToLogger() {
    static char messages[16][100];
    int msgCount = 0;

    while (smLoggerQueue_->count() > 0) {
        smLoggerQueue_->get(&messages[msgCount]);
        msgCount++;
    }

    logger->log(messages, msgCount);
}
