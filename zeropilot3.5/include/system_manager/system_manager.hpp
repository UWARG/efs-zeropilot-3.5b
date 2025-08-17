#pragma once

#include "iwdg_iface.hpp"
#include "logger_iface.hpp"
#include "rc_iface.hpp"
#include "rc_motor_control.hpp"
#include "queue_iface.hpp"

#define SM_MAIN_DELAY 50

class SystemManager {
    public:
        SystemManager(
            IIndependentWatchdog *iwdgDriver,
            ILogger *loggerDriver,
            IRCReceiver *rcDriver,
            IMessageQueue<RCMotorControlMessage_t> *amRCQueue,
            IMessageQueue<char[100]> *smLoggerQueue
        );

        void smUpdate(); // This function is the main function of SM, it should be called in the main loop of the system.

    private:
        IIndependentWatchdog *iwdgDriver; // Independent Watchdog driver
        ILogger *loggerDriver; // Logger driver
        IRCReceiver *rcDriver; // RC receiver driver

        IMessageQueue<RCMotorControlMessage_t> *amRcQueue; // Queue driver for communication to the Attitude Manager
        IMessageQueue<char[100]> *smLoggerQueue;

        void sendRCDataToAttitudeManager(const RCControl &rcData);
        void sendMessagesToLogger();
};
