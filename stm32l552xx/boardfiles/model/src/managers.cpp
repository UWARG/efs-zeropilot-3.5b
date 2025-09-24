#include "direct_mapping.hpp"
#include "drivers.hpp"
#include "managers.hpp"

DirectMapping *flightMode = nullptr;
AttitudeManager *amHandle = nullptr;

Logger *loggerHandle = nullptr;
Config *configHandle = nullptr;
IMessageQueue<ConfigMessage_t> *smConfigRouteQueueHandle[static_cast<size_t>(Owner::COUNT)] = {nullptr};
SystemManager *smHandle = nullptr;



void initManagers()
{
    // AM initialization

    flightMode = new DirectMapping();
    amHandle = new AttitudeManager(amRCQueueHandle, smLoggerQueueHandle, smConfigAttitudeQueueHandle, flightMode, &rollMotors, &pitchMotors, &yawMotors, &throttleMotors, &flapMotors, &steeringMotors);

    // SM initialization
    loggerHandle = new Logger(textIOHandle);
    configHandle = new Config(configTextIOHandle);
    smConfigRouteQueueHandle[static_cast<size_t>(Owner::ATTITUDE_MANAGER)] = smConfigAttitudeQueueHandle;

    smHandle = new SystemManager(iwdgHandle, rcHandle, amRCQueueHandle, smLoggerQueueHandle, smConfigQueueHandle, smConfigRouteQueueHandle, loggerHandle, configHandle);
}
