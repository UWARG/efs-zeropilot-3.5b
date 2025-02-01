#include "system_manager.hpp"

SystemManager::SystemManager(IRCReceiver *rcDriver, IMessageQueue<RCMotorControlMessage_t> *amQueueDriver, IMessageQueue<RCMotorControlMessage_t> *smQueueDriver, uint32_t invalidThreshold)
    : rcDriver_(rcDriver), amQueueDriver_(amQueueDriver), smQueueDriver_(smQueueDriver), invalidRCCount_(invalidThreshold) {}

void SystemManager::SMUpdate() {
    RCControl rcData = rcDriver_->getRCData();
    
    // If no new data is recieved for some time, go to failsafe
    if (!rcData.isDataNew) {
        invalidRCCount_++;
        if (invalidRCCount_ > invalidRCCount_) {
            sendDisarmedToAttitudeManager();
        }
    } 
    else {        
        invalidRCCount_ = 0;
        sendRCDataToAttitudeManager(rcData);
    }
}

void SystemManager::sendRCDataToAttitudeManager(const RCControl &rcData) {
    RCMotorControlMessage_t rcDataMessage;

    rcDataMessage.roll = rcData.roll;
    rcDataMessage.pitch = rcData.pitch;
    rcDataMessage.yaw = rcData.yaw;
    rcDataMessage.throttle = rcData.throttle;

    amQueueDriver_->push(rcDataMessage);
}

void SystemManager::sendDisarmedToAttitudeManager() {
    RCMotorControlMessage_t disarmedMessage;

    disarmedMessage.roll = 0.0f;
    disarmedMessage.pitch = 0.0f;
    disarmedMessage.yaw = 0.0f;
    disarmedMessage.throttle = -1.0f;

    amQueueDriver_->push(disarmedMessage);
}
