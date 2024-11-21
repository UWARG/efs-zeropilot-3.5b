#include "system_manager.hpp"

SystemManager::SystemManager(SBusIface *rc_driver, QueueIface<RCMotorControlMessage_t> *queue_driver)
    : rc_driver_(rc_driver), queue_driver_(queue_driver) {}

SystemManager::~SystemManager() {}

void SystemManager::SMUpdate() {
    // TODO: Implement this function
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
