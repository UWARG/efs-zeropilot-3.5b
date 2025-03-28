#pragma once

#include <cstdint>
#include "logger_iface.hpp"
#include "queue_iface.hpp"
#include "rc_iface.hpp"
#include "rc_motor_control.hpp"
#include "iwdg_iface.hpp"

#define mainLoopFreq 50

class SystemManager {
    public:
        SystemManager(
            IIndependentWatchdog *iwdgDriver,
            ILogger *loggerDriver,
            IRCReceiver *rcDriver, 
            IMessageQueue<RCMotorControlMessage_t> *amQueueDriver, 
            IMessageQueue<RCMotorControlMessage_t> *smQueueDriver, 
            IMessageQueue<char[100]> *loggerQueueDriver
        );

        void SMUpdate(); // This function is the main function of SM, it should be called in the main loop of the system.

    private:
        IIndependentWatchdog *iwdgDriver_; // Independent Watchdog driver
        ILogger *loggerDriver_;
        IRCReceiver *rcDriver_; // RC receiver driver
        
        IMessageQueue<RCMotorControlMessage_t> *amQueueDriver_; // Queue driver for communication to the Attitude Manager
        IMessageQueue<RCMotorControlMessage_t> *smQueueDriver_; // RCMotorControlMessage_t is a placeholder until the actual message type is defined.
        IMessageQueue<char[100]> *loggerQueueDriver_;

        void sendRCDataToAttitudeManager(const RCControl &rcData);
        void sendMessageToLogger();
};
