#pragma once

#include <cstdint>
#include "logger_iface.hpp"
#include "queue_iface.hpp"
#include "rc_iface.hpp"
#include "rc_datatypes.hpp"
#include "rc_motor_control.hpp"
#include "iwdg_iface.hpp"

#define SM_MAIN_DELAY 50

class SystemManager {
    public:
        SystemManager(
            //IIndependentWatchdog *iwdgDriver,
            ILogger *loggerDriver,
            IRCReceiver *rcDriver, 
            IMessageQueue<RCMotorControlMessage_t> *amRCQueue,
            IMessageQueue<char[100]> *smLoggerQueue
        );

        void SMUpdate(); // This function is the main function of SM, it should be called in the main loop of the system.

    private:
        //IIndependentWatchdog *iwdgDriver_; // Independent Watchdog driver
        ILogger *loggerDriver_; // Logger driver
        IRCReceiver *rcDriver_; // RC receiver driver
        
        IMessageQueue<RCMotorControlMessage_t> *amRCQueue_; // Queue driver for communication to the Attitude Manager
        IMessageQueue<char[100]> *smLoggerQueue_;

        void sendRCDataToAttitudeManager(const RCControl &rcData);
        void sendMessagesToLogger();
};
