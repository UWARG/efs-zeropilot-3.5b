#include "system_manager.hpp"

namespace SM {

SystemManager::SystemManager(SBusIface *rc_driver, QueueIface<AM::AttitudeManager::RCMotorControlMessage_t> *queue_driver)
    : rc_driver_(rc_driver), queue_driver_(queue_driver) {}

SystemManager::~SystemManager() {}

void SystemManager::SMUpdate() {
    // TODO: Implement this function
}

void SystemManager::sendRCDataToAttitudeManager(const SBusIface::RCData_t &rcData) {
    // TODO: Implement this function
}

void SystemManager::sendDisarmedToAttitudeManager() {
    // TODO: Implement this function
}

} // namespace SM