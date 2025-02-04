#pragma once

#include <cstdint>
#include "queue_iface.hpp"
#include "rc_iface.hpp"
#include "rc_motor_control.hpp"
#include "idwg_iface.hpp"

class SystemManager {
    public:
        SystemManager(IRCReceiver *rcDriver, IMessageQueue<RCMotorControlMessage_t> *amQueueDriver, IMessageQueue<RCMotorControlMessage_t> *smQueueDriver);

        void SMUpdate(); // This function is the main function of SM, it should be called in the main loop of the system.

    private:
        IIndependentWatchdog *iwdg_; // Independent Watchdog driver
        IRCReceiver *rcDriver_; // RC receiver driver
        IMessageQueue<RCMotorControlMessage_t> *amQueueDriver_; // Queue driver for communication to the Attitude Manager
        IMessageQueue<RCMotorControlMessage_t> *smQueueDriver_; // RCMotorControlMessage_t is a placeholder until the actual message type is defined.

        void sendRCDataToAttitudeManager(const RCControl &rcData);
};
