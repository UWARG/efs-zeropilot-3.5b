#include "system_manager.hpp"

SystemManager::SystemManager(IRCReceiver *rcDriver, IMessageQueue<RCMotorControlMessage_t> *queueDriver, uint32_t invalidThreshold)
    : rcDriver_(rcDriver), queueDriver_(queueDriver), invalidRCCount_(invalidThreshold) {}

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
    if (rcData.arm == 0.0f) {
        sendDisarmedToAttitudeManager();
    }
    else {
        RCMotorControlMessage_t rcDataMessage;

        rcDataMessage.roll = rcData.roll;
        rcDataMessage.pitch = rcData.pitch;
        rcDataMessage.yaw = rcData.yaw;
        rcDataMessage.throttle = rcData.throttle;
        rcDataMessage.arm = rcData.arm;

        queueDriver_->push(rcDataMessage);
    }
}

void SystemManager::sendDisarmedToAttitudeManager() {
    RCMotorControlMessage_t disarmedMessage;

    disarmedMessage.roll = 0.0f;
    disarmedMessage.pitch = 0.0f;
    disarmedMessage.yaw = 0.0f;
    disarmedMessage.throttle = 0.0f;
    disarmedMessage.arm = 0.0f;

    queueDriver_->push(disarmedMessage);
}
