#include "system_manager.hpp"

SystemManager::SystemManager(
    IIndependentWatchdog *iwdgDriver,
    IRCReceiver *rcDriver, 
    IMessageQueue<RCMotorControlMessage_t> *amRCQueue,
    IMessageQueue<char[100]> *smLoggerQueue,
    IMessageQueue<ConfigMessage_t> *smConfigQueue,
    IMessageQueue<ConfigMessage_t> *smConfigRouteQueue[static_cast<size_t>(Owner::COUNT)],
    Logger *logger,
    Config *config) :
        iwdgDriver_(iwdgDriver),
        rcDriver_(rcDriver), 
        amRCQueue_(amRCQueue),
        smLoggerQueue_(smLoggerQueue),
        smConfigQueue_(smConfigQueue),
        smConfigRouteQueue_(),
        logger(logger),
        config(config) {
            for (size_t i = 0; i < static_cast<size_t>(Owner::COUNT); ++i) {
                smConfigRouteQueue_[i] = smConfigRouteQueue[i];
            }
        }

void SystemManager::SMUpdate() {
    // Kick the watchdog
    iwdgDriver_->refreshWatchdog();

    // Get RC data from the RC receiver and passthrough to AM if new
    static int oldDataCount = 0;
    static bool rcConnected = true;

    RCControl rcData = rcDriver_->getRCData();
    if (rcData.isDataNew) {
        oldDataCount = 0;
        sendRCDataToAttitudeManager(rcData);

        if (!rcConnected) {
            logger->log("RC Reconnected");
            rcConnected = true;
        }
    } else {
        oldDataCount += 1;

        if ((oldDataCount * SM_MAIN_DELAY > 500) && rcConnected) {
            logger->log("RC Disconnected");
            rcConnected = false;
        }
    }

    // Somehow recieve config messages from telemetry manager and send to config manager here once Telemetry Manager is implemented

    // Log if new messages
    if (smLoggerQueue_->count() > 0) {
        sendMessagesToLogger();
    }
}

void SystemManager::sendRCDataToAttitudeManager(const RCControl &rcData) {
    RCMotorControlMessage_t rcDataMessage;

    rcDataMessage.roll = rcData.roll;
    rcDataMessage.pitch = rcData.pitch;
    rcDataMessage.yaw = rcData.yaw;
    rcDataMessage.throttle = rcData.throttle;
    rcDataMessage.arm = rcData.arm;
    rcDataMessage.flapAngle = rcData.aux2;

    amRCQueue_->push(&rcDataMessage);
}

void SystemManager::sendMessagesToLogger() {
    static char messages[16][100];
    int msgCount = 0;

    while (smLoggerQueue_->count() > 0) {
        smLoggerQueue_->get(&messages[msgCount]);
        msgCount++;
    }

    logger->log(messages, msgCount);
}

// Note that I am using ConfigMessage_t here as a placeholder for the parameter as we might have more things from whatever comes from Telemetry Manager that for different purposes that I dont know about yet
void SystemManager::sendMessagesToConfigManager() {
    static ConfigMessage_t configMessages[16];
    int msgCount = 0;
    while (smConfigQueue_->count() > 0) {
        smConfigQueue_->get(&configMessages[msgCount]);
        msgCount++;
    }

    for (int i = 0; i < msgCount; i++) {
        int result = config->writeParam(static_cast<ConfigKey>(configMessages[i].key), configMessages[i].value);
        if (result != 0) {
            logger->log("Error writing config param " + configMessages[i].key);
        } else {
            // Add message to the updateParam queue for other managers to update their local copy of the param if needed
            Owner owner = config->getParamOwner(static_cast<ConfigKey>(configMessages[i].key));
            if (owner >= Owner::COUNT) {
                logger->log("Invalid owner for config param " + configMessages[i].key);
                continue;
            }
            smConfigRouteQueue_[static_cast<size_t>(owner)]->push(&configMessages[i]);
        }
    }
}
