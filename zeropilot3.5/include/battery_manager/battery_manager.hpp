#pragma once

#include <cstdint>
#include <string>
#include "queue_iface.hpp"
#include "rc_iface.hpp"
#include "rc_motor_control.hpp"
#include "idwg_iface.hpp"
#include "driver_ifaces/bt_iface.hpp"
#include "driver_ifaces/battery_datatypes.hpp"

class BatteryManager {
    public:
        BatteryManager(float BATT_LOW_VOLT, float BATT_LOW_MAH, float BATT_CRITICAL_VOLT, float BATT_CRITICAL_MAH, 
                      IBatteryReceiver *bDriver, IMessageQueue<RCMotorControlMessage_t> *amQueueDriver, 
                      IMessageQueue<RCMotorControlMessage_t> *smQueueDriver, IIndependentWatchdog *iwdg);

        float BATT_LOW_VOLT;
        float BATT_LOW_MAH; 
        float BATT_CRITICAL_VOLT;
        float BATT_CRITICAL_MAH; 

        void BMUpdate(); // This function updates battery status
        std::string Battery_Logic_Detection(const BatteryData &batteryData);

    private:
        IIndependentWatchdog *iwdg_; // Independent Watchdog driver

        IBatteryReceiver *bDriver_; // Battery Driver interface
        IMessageQueue<RCMotorControlMessage_t> *amQueueDriver_; // Queue driver for communication to the Attitude Manager
        IMessageQueue<RCMotorControlMessage_t> *smQueueDriver_; // Queue driver for communication to the System Manager

        void sendRCDataToAttitudeManager(const BatteryData &batteryData);
};
