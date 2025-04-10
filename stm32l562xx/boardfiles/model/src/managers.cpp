#include "direct_mapping.hpp"
#include "drivers.hpp"
#include "managers.hpp"

AttitudeManager *amHandle = nullptr;
SystemManager *smHandle = nullptr;
DirectMapping *flightMode = nullptr;


void initManagers()
{
    // AM initialization

    flightMode = new DirectMapping();
    amHandle = new AttitudeManager(amRCQueueHandle, smLoggerQueueHandle, flightMode, &rollMotors, &pitchMotors, &yawMotors, &throttleMotors);

    // SM initialization
    smHandle = new SystemManager(loggerHandle, rcHandle, amRCQueueHandle, smLoggerQueueHandle);
}
