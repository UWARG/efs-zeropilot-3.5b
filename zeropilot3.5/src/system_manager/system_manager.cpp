#include "system_manager.hpp"

SystemManager::SystemManager(SBusIface *rc_driver, QueueIface<RCMotorControlMessage_t> *queue_driver, int16_t invalid_threshold)
    : rc_driver_(rc_driver), queue_driver_(queue_driver), invalid_threshold_(invalid_threshold) {}

SystemManager::~SystemManager() {}

void SystemManager::SMUpdate() {
    // If no new data is recieved for some time, go to failsafe
    if (!rc_driver_->is_data_new()) {
        invalidRCCount_++;

        if (invalidRCCount_ > invalid_threshold_) {
            sendDisarmedToAttitudeManager();
        }
    } 

    else {
        RCControl rcData = rc_driver_->getRCData();
        sendRCDataToAttitudeManager(rcData);
        invalidRCCount_ = 0;
    }
}

void SystemManager::sendRCDataToAttitudeManager(const SBusIface::RCControl &rcData) {
    RCMotorControlMessage_t rcDataMessage;

    rcDataMessage.roll = rcData.roll;
    rcDataMessage.pitch = rcData.pitch;
    rcDataMessage.yaw = rcData.yaw;
    rcDataMessage.throttle = rcData.throttle;

    queue_driver_->push(rcDataMessage);
}

void SystemManager::sendDisarmedToAttitudeManager() {
    // TODO: Implement this function
}
