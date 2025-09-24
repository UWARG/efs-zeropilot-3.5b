#pragma once

#include <cstdint>
#include "logger.hpp"
#include "config.hpp"
#include "queue_iface.hpp"
#include "rc_iface.hpp"
#include "rc_datatypes.hpp"
#include "rc_motor_control.hpp"
#include "config_msg.hpp"
#include "iwdg_iface.hpp"

#define SM_MAIN_DELAY 50

class SystemManager {
    public:
        SystemManager(
            IIndependentWatchdog *iwdgDriver,
            IRCReceiver *rcDriver, 
            IMessageQueue<RCMotorControlMessage_t> *amRCQueue,
            IMessageQueue<char[100]> *smLoggerQueue,
            IMessageQueue<ConfigMessage_t> *smConfigQueue,
            IMessageQueue<ConfigMessage_t> *smConfigRouteQueue[static_cast<size_t>(Owner::COUNT)],
            Logger *logger,
            Config *config
        );

        void SMUpdate(); // This function is the main function of SM, it should be called in the main loop of the system.

    private:
        IIndependentWatchdog *iwdgDriver_; // Independent Watchdog driver
        IRCReceiver *rcDriver_; // RC receiver driver
        
        IMessageQueue<RCMotorControlMessage_t> *amRCQueue_; // Queue driver for communication to the Attitude Manager
        IMessageQueue<char[100]> *smLoggerQueue_;
        IMessageQueue<ConfigMessage_t> *smConfigQueue_; // Queue driver for communication with Config Manager from Telemetry Manager
        IMessageQueue<ConfigMessage_t> *smConfigRouteQueue_[static_cast<size_t>(Owner::COUNT)]; // Array of queues for routing config messages to respective managers

        Logger *logger; // Logger

        Config *config; // Config manager

        void sendRCDataToAttitudeManager(const RCControl &rcData);
        void sendMessagesToLogger();
        void sendMessagesToConfigManager();
};
