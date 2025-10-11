#include "direct_mapping.hpp"
#include "drivers.hpp"
#include "managers.hpp"

AttitudeManager *amHandle = nullptr;
SystemManager *smHandle = nullptr;
TelemetryManager *tmHandle = nullptr;
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

    // AM initialization
    amHandle = new AttitudeManager(systemUtilsHandle, gpsHandle, amRCQueueHandle, tmQueueHandle, smLoggerQueueHandle, smConfigAttitudeQueueHandle, flightMode, &rollMotors, &pitchMotors, &yawMotors, &throttleMotors, &flapMotors, &steeringMotors);

    // SM initialization
    loggerHandle = new Logger(textIOHandle);
    configHandle = new Config(textIOHandle);
    smConfigRouteQueueHandle[static_cast<size_t>(Owner::ATTITUDE_MANAGER)] = smConfigAttitudeQueueHandle;
    smHandle = new SystemManager(systemUtilsHandle, iwdgHandle, rcHandle, amRCQueueHandle, tmQueueHandle, smLoggerQueueHandle, smConfigQueueHandle, smConfigRouteQueueHandle, loggerHandle, configHandle);

    // TM initialization
    tmHandle = new TelemetryManager(systemUtilsHandle, rfdHandle, tmQueueHandle, amRCQueueHandle, messageBufferHandle);
}
