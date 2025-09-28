#include "system_manager.hpp"

SystemManager::SystemManager(
    IIndependentWatchdog *iwdgDriver,
    ILogger *loggerDriver,
    IRCReceiver *rcDriver,
    IGPS *gpsDriver,
    IMessageQueue<RCMotorControlMessage_t> *amRCQueue,
    IMessageQueue<TMMessage_t> *tmQueue,
    IMessageQueue<char[100]> *smLoggerQueue) :
        iwdgDriver(iwdgDriver),
        loggerDriver(loggerDriver),
        rcDriver(rcDriver),
        gpsDriver(gpsDriver),
        amRCQueue(amRCQueue),
        tmQueue(tmQueue),
        smLoggerQueue(smLoggerQueue) {}

void SystemManager::smUpdate() {
    // Kick the watchdog
    iwdgDriver->refreshWatchdog();

    // Get RC data from the RC receiver and passthrough to AM if new
    static int oldDataCount = 0;
    static bool rcConnected = true;

    RCControl rcData = rcDriver->getRCData();
    if (rcData.isDataNew) {
        oldDataCount = 0;
        sendRCDataToAttitudeManager(rcData);

        if (!rcConnected) {
            loggerDriver->log("RC Reconnected");
            rcConnected = true;
        }
    } else {
        oldDataCount += 1;

        if ((oldDataCount * SM_MAIN_DELAY > 500) && rcConnected) {
            loggerDriver->log("RC Disconnected");
            rcConnected = false;
        }
    }

    GpsData_t gpsData = gpsDriver->readData();

    // Send RC data to TM
    sendRCDataToTelemetryManager(rcData);

    // Send GPS data to TM
    sendGPSDataToTelemetryManager(gpsData);

    // Log if new messages
    if (smLoggerQueue->count() > 0) {
        sendMessagesToLogger();
    }
}

void SystemManager::sendRCDataToTelemetryManager(const RCControl &rcData) {
    TMMessage_t rcDataMsg =  rcDataPack(0, rcData.roll, rcData.pitch, rcData.yaw, rcData.throttle, rcData.aux2, rcData.arm);
    tmQueue->push(&rcDataMsg);
}

void SystemManager::sendRCDataToAttitudeManager(const RCControl &rcData) {
    RCMotorControlMessage_t rcDataMessage;

    rcDataMessage.roll = rcData.roll;
    rcDataMessage.pitch = rcData.pitch;
    rcDataMessage.yaw = rcData.yaw;
    rcDataMessage.throttle = rcData.throttle;
    rcDataMessage.arm = rcData.arm;
    rcDataMessage.flapAngle = rcData.aux2;

    amRCQueue->push(&rcDataMessage);
}

void SystemManager::sendGPSDataToTelemetryManager(const GpsData_t &gpsData) {
    TMMessage_t gpsDataMsg1 = gposDataPack(
        0, 0, 100000000, 100000000, 0, 0, 0, 0, 0
    );
    tmQueue->push(&gpsDataMsg1);

    return;
    if (!gpsData.isNew) return;

    TMMessage_t gpsDataMsg = gposDataPack(
        0, // time_boot_ms
        0, // alt
        gpsData.latitude * 1e7,
        gpsData.longitude * 1e7,
        0, // relative altitude
        gpsData.vx,
        gpsData.vy,
        0, // vz
        gpsData.trackAngle
    );

    tmQueue->push(&gpsDataMsg);
}

void SystemManager::sendMessagesToLogger() {
    static char messages[16][100];
    int msgCount = 0;

    while (smLoggerQueue->count() > 0) {
        smLoggerQueue->get(&messages[msgCount]);
        msgCount++;
    }

    loggerDriver->log(messages, msgCount);
}
