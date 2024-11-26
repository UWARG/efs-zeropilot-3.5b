#include "system_manager.hpp"

SystemManager::SystemManager(SBusIface *rc_driver, QueueIface<RCMotorControlMessage_t> *queue_driver, int16_t invalid_threshold)
    : rc_driver_(rc_driver), queue_driver_(queue_driver), invalid_threshold_(invalid_threshold) {}

SystemManager::~SystemManager() {}

void SystemManager::SMUpdate() {
    // TODO: Implement this function

    // If no new data is recieved for some time, go to failsafe
    if (!rc_driver_->is_data_new()) {
        invalidRCCount_++;

        if (invalidRCCount_ > invalid_threshold_) {
            sendDisarmedToAttitudeManager();
        }
    } 

    else {
        sendRCDataToAttitudeManager(rc_driver_);
        invalidRCCount_ = 0;
    }
}

void SystemManager::sendRCDataToAttitudeManager(const SBusIface::RCData_t &rcData) {
    // TODO: Implement this function

    RCControl control_data = rcData->getRCData();
    RCMotorControlMessage_t rcDataMessage;

    rcDataMessage.roll = control_data.roll;
    rcDataMessage.pitch = control_data.pitch;
    rcDataMessage.yaw = control_data.yaw;
    rcDataMessage.throttle = control_data.throttle;

    queue_driver_->push(rcDataMessage);
}

void SystemManager::sendDisarmedToAttitudeManager() {
    // TODO: Implement this function
}
