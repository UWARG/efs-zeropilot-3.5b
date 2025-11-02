#include "direct_mapping.hpp"
#include "drivers.hpp"
#include "managers.hpp"

AttitudeManager *amHandle = nullptr;
SystemManager *smHandle = nullptr;
TelemetryManager *tmHandle = nullptr;

void initManagers()
{
    // AM initialization
    amHandle = new AttitudeManager(systemUtilsHandle, gpsHandle, amRCQueueHandle, tmQueueHandle, smLoggerQueueHandle, &rollMotors, &pitchMotors, &yawMotors, &throttleMotors, &flapMotors, &steeringMotors);

    // SM initialization
    smHandle = new SystemManager(systemUtilsHandle, iwdgHandle, loggerHandle, rcHandle, amRCQueueHandle, tmQueueHandle, smLoggerQueueHandle);

    // TM initialization
    tmHandle = new TelemetryManager(systemUtilsHandle, rfdHandle, tmQueueHandle, amRCQueueHandle, messageBufferHandle);
}
