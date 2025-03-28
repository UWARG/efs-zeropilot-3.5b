#include "system_manager.hpp"

SystemManager::SystemManager(
    IIndependentWatchdog *iwdgDriver,
    ILogger *loggerDriver,
    IRCReceiver *rcDriver, 
    IMessageQueue<RCMotorControlMessage_t> *amRCQueueDriver, 
    IMessageQueue<char[100]> *loggerQueueDriver) : 
        iwdgDriver_(iwdgDriver),
        loggerDriver_(loggerDriver),
        rcDriver_(rcDriver), 
        amRCQueueDriver_(amRCQueueDriver),
        loggerQueueDriver_(loggerQueueDriver) {}

void SystemManager::SMUpdate() {
    // Kick the watchdog
    iwdgDriver_->refreshWatchdog();

    // Get RC data from the RC receiver and passthrough to AM if new
    static int oldDataCount = 0;

    RCControl rcData = rcDriver_->getRCData();
    if (rcData.isDataNew) {
        oldDataCount = 0;
        sendRCDataToAttitudeManager(rcData);
    } else {
        oldDataCount += 1;

        if ((oldDataCount / mainLoopFreq) > 5) {
            loggerDriver_->log("RC Disconnect");
        }
    }

    // Log if new messages
    if (loggerQueueDriver_->count() > 0) {
        sendMessageToLogger();
    }
}

void SystemManager::sendRCDataToAttitudeManager(const RCControl &rcData) {
    RCMotorControlMessage_t rcDataMessage;

    rcDataMessage.roll = rcData.roll;
    rcDataMessage.pitch = rcData.pitch;
    rcDataMessage.yaw = rcData.yaw;
    rcDataMessage.throttle = rcData.throttle;
    rcDataMessage.arm = rcData.arm;

    amRCQueueDriver_->push(&rcDataMessage);
}

void SystemManager::sendMessageToLogger() {
    char message[100];
    loggerQueueDriver_->get(&message);

    loggerDriver_->log(message);
}