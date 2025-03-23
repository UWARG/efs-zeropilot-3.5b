#include "battery_manager.hpp"

BatteryManager::BatteryManager(float BATT_LOW_VOLT, float BATT_LOW_MAH, float BATT_CRITICAL_VOLT, float BATT_CRITICAL_MAH, 
                              IBatteryReceiver *bDriver, IMessageQueue<RCMotorControlMessage_t> *amQueueDriver, 
                              IMessageQueue<RCMotorControlMessage_t> *smQueueDriver, IIndependentWatchdog *iwdg)
    : BATT_LOW_VOLT(BATT_LOW_VOLT), 
      BATT_LOW_MAH(BATT_LOW_MAH),
      BATT_CRITICAL_VOLT(BATT_CRITICAL_VOLT),
      BATT_CRITICAL_MAH(BATT_CRITICAL_MAH),
      bDriver_(bDriver), 
      amQueueDriver_(amQueueDriver),
      smQueueDriver_(smQueueDriver),
      iwdg_(iwdg) {
}

void BatteryManager::BMUpdate() {
    // Kick the watchdog 
    iwdg_->refreshWatchdog();

    // Get battery data
    BatteryData batteryData = bDriver_->getBatteryData();
    if (batteryData.isDataNew) {
        Battery_Logic_Detection(batteryData);
    }
}

std::string BatteryManager::Battery_Logic_Detection(const BatteryData &batteryData) {
    // Check voltage levels
    if (batteryData.voltage <= BATT_CRITICAL_VOLT) {
        // send autoland command
        return "Battery voltage critical - initiating autoland";
    }
    if (batteryData.voltage <= BATT_LOW_VOLT) {
        // send warning
        return "Battery voltage low";
    }

    // Check capacity levels
    if (batteryData.capacity <= BATT_CRITICAL_MAH) {
        // send autoland command
        return "Battery capacity critical - initiating autoland";
    }
    if (batteryData.capacity <= BATT_LOW_MAH) {
        // send warning
        return "Battery capacity low";
    

    // Everything is okay
    return "Battery status OK";
}