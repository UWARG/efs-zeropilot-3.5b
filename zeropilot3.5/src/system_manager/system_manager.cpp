#include "system_manager.hpp"

SystemManager::SystemManager(IRCReceiver *rcDriver, IMessageQueue<RCMotorControlMessage_t> *amQueueDriver, IMessageQueue<RCMotorControlMessage_t> *smQueueDriver)
    : rcDriver_(rcDriver), amQueueDriver_(amQueueDriver), smQueueDriver_(smQueueDriver) {}

void SystemManager::SMUpdate() {
    RCControl rcData = rcDriver_->getRCData();
    
    // Only passthrough RC data if it is new
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
