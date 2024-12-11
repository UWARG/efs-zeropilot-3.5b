#include "system_manager.hpp"

SystemManager::SystemManager(ISBUSReceiver *rcDriver, IMessageQueue<RCMotorControlMessage_t> *queueDriver, uint32_t invalidThreshold)
    : rcDriver_(rcDriver), queueDriver_(queueDriver), invalidRCCount_(invalidThreshold) {}

void SystemManager::SMUpdate() {
    RCControl_t rcData = rcDriver_->getRCData();
    
    // If no new data is recieved for some time, go to failsafe
    if (!rcData.isDataNew) {
        invalidRCCount_++;
        if (invalidRCCount_ > invalidRCCount_) {
            sendDisarmedToAttitudeManager();
        }
    } 
    else {
        invalidRCCount_ = 0;
    }
}

void SystemManager::sendRCDataToAttitudeManager(const RCControl_t &rcData) {
    // TODO: Implement this function
}

void SystemManager::sendDisarmedToAttitudeManager() {
    RCMotorControlMessage_t disarmedMessage;

    disarmedMessage.roll = 0.0f;
    disarmedMessage.pitch = 0.0f;
    disarmedMessage.yaw = 0.0f;
    disarmedMessage.throttle = -1.0f;

    queueDriver_->push(disarmedMessage);
}
