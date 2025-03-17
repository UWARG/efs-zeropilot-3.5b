#pragma once

#include <cstdint>
#include <string>
#include "queue_iface.hpp"
#include "rc_iface.hpp"
#include "rc_motor_control.hpp"
#include "idwg_iface.hpp"

class BatteryManager {
    public:
        BatteryManager(float BATT_LOW_VOLT, float BATT_LOW_MAH, float BATT_CRITICAL_VOLT, float BATT_CRITICAL_MAH, 
                      IBatteryReciever *bDriver, IMessageQueue<RCMotorControlMessage_t> *amQueueDriver, 
                      IMessageQueue<RCMotorControlMessage_t> *smQueueDriver, IIndependentWatchdog *iwdg);

        float BATT_LOW_VOLT;
        float BATT_LOW_MAH; 
        float BATT_CRITICAL_VOLT;
        float BATT_CRITICAL_MAH; 

        void BMUpdate(); // This function updates battery status
        std::string Battery_Logic_Detection(BatteryCntr &batteryData);

    private:
        IIndependentWatchdog *iwdg_; // Independent Watchdog driver

        IBatteryReciever *bDriver_; // Battery Driver Not defined yet change if name is different when defined 
        IMessageQueue<RCMotorControlMessage_t> *smQueueDriver_; // same  RCMotorControlMessage_t is a placeholder until the actual message type is defined.

        void sendRCDataToAttitudeManager(const BatteryCntr &batteryData);
};
