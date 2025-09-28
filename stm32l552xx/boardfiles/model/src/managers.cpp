#include "direct_mapping.hpp"
#include "drivers.hpp"
#include "managers.hpp"

AttitudeManager *amHandle = nullptr;
SystemManager *smHandle = nullptr;
TelemetryManager *tmHandle = nullptr;
DirectMapping *flightMode = nullptr;


void initManagers()
{
    // AM initialization

    flightMode = new DirectMapping();
    amHandle = new AttitudeManager(amRCQueueHandle, smLoggerQueueHandle, flightMode, &rollMotors, &pitchMotors, &yawMotors, &throttleMotors, &flapMotors, &steeringMotors);

    // SM initialization
    smHandle = new SystemManager(iwdgHandle, loggerHandle, rcHandle, gpsHandle, amRCQueueHandle, tmQueueHandle, smLoggerQueueHandle);

    // TM initialization
    tmHandle = new TelemetryManager(rfdHandle, tmQueueHandle, amRCQueueHandle, messageBufferHandle);
}
