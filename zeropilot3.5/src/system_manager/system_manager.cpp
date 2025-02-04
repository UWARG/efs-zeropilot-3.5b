#include "system_manager.hpp"

SystemManager::SystemManager(IRCReceiver *rcDriver, IMessageQueue<RCMotorControlMessage_t> *amQueueDriver, IMessageQueue<RCMotorControlMessage_t> *smQueueDriver)
    : rcDriver_(rcDriver), amQueueDriver_(amQueueDriver), smQueueDriver_(smQueueDriver) {}

void SystemManager::SMUpdate() {
    // Kick the watchdog
    iwdg_->refreshWatchdog();

    // Get RC data from the RC receiver and passthrough to AM if new
    RCControl rcData = rcDriver_->getRCData();
    if (rcData.isDataNew) {
        sendRCDataToAttitudeManager(rcData);
    }
}

void SystemManager::sendRCDataToAttitudeManager(const RCControl &rcData) {
    RCMotorControlMessage_t rcDataMessage;

    rcDataMessage.roll = rcData.roll;
    rcDataMessage.pitch = rcData.pitch;
    rcDataMessage.yaw = rcData.yaw;
    rcDataMessage.throttle = rcData.throttle;
    rcDataMessage.arm = rcData.arm;

    amQueueDriver_->push(rcDataMessage);
}
