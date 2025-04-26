#include "direct_mapping.hpp"
#include "drivers.hpp"
#include "managers.hpp"

DirectMapping *flightMode = nullptr;
AttitudeManager *amHandle = nullptr;
SystemManager *smHandle = nullptr;

void initManagers()
{
    // AM initialization
    flightMode = new DirectMapping();
    amHandle = new AttitudeManager(
        amRCQueueHandle, 
        smLoggerQueueHandle, 
        flightMode, 
        &rollMotors, 
        &pitchMotors, 
        &yawMotors, 
        &throttleMotors, 
        &flapMotors, 
        &steeringMotors
    );

    // SM initialization
    smHandle = new SystemManager(
        iwdgHandle,
        loggerHandle, 
        rcHandle, 
        amRCQueueHandle, 
        smLoggerQueueHandle
    );
}
