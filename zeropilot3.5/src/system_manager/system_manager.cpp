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
        invalidRCCount_ = 0;
    }

}

void SystemManager::sendRCDataToAttitudeManager(const SBusIface::RCData_t &rcData) {
    // TODO: Implement this function
}

void SystemManager::sendDisarmedToAttitudeManager() {
    RCMotorControlMessage_t disarmedMessage;

    disarmedMessage.roll = 0.0f;
    disarmedMessage.pitch = 0.0f;
    disarmedMessage.yaw = 0.0f;
    disarmedMessage.throttle = -1.0f;

    queue_driver_->push(disarmedMessage);
}
