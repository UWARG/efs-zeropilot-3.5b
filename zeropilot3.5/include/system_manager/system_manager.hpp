#pragma once

#include <cstdint>
#include "logger_iface.hpp"
#include "queue_iface.hpp"
#include "rc_iface.hpp"
#include "rc_datatypes.hpp"
#include "rc_motor_control.hpp"
#include "iwdg_iface.hpp"
#include "tm_queue.hpp"

#define SM_MAIN_DELAY 50

class SystemManager {
    public:
        SystemManager(
            IIndependentWatchdog *iwdgDriver,
            ILogger *loggerDriver,
            IRCReceiver *rcDriver,
            IMessageQueue<RCMotorControlMessage_t> *amRCQueue,
            IMessageQueue<TMMessage_t> *tmQueue,
            IMessageQueue<char[100]> *smLoggerQueue
        );

        void smUpdate(); // This function is the main function of SM, it should be called in the main loop of the system.

    private:
        IIndependentWatchdog *iwdgDriver; // Independent Watchdog driver
        ILogger *loggerDriver; // Logger driver
        IRCReceiver *rcDriver; // RC receiver driver
        
        IMessageQueue<RCMotorControlMessage_t> *amRCQueue; // Queue driver for tx communication to the Attitude Manager
        IMessageQueue<TMMessage_t> *tmQueue; // Queue driver for tx communication to the Telemetry Manager
        IMessageQueue<char[100]> *smLoggerQueue; // Queue driver fpr rx communication from other modules to the System Manager for logging

        void sendRCDataToAttitudeManager(const RCControl &rcData);
        void sendRCDataToTelemetryManager(const RCControl &rcData);
        void sendMessagesToLogger();
};
