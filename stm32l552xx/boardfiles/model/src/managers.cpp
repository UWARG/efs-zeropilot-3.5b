#include "direct_mapping.hpp"
#include "drivers.hpp"
#include "managers.hpp"

AttitudeManager *amHandle = nullptr;
SystemManager *smHandle = nullptr;
DirectMapping *flightMode = nullptr;


void initManagers()
{
    // AM initialization
    MotorInstance_t rollMotorInstance[2] = {
        {leftAileronMotorHandle, false}, 
        {rightAileronMotorHandle, false}
    };
    MotorInstance_t elevatorMotorInstance = {elevatorMotorHandle, false};
    MotorInstance_t rudderMotorInstance = {rudderMotorHandle, false};
    MotorInstance_t throttleMotorInstance = {throttleMotorHandle, false};

    MotorGroupInstance_t rollMotors = {rollMotorInstance, 2};
    MotorGroupInstance_t pitchMotors = {&elevatorMotorInstance, 1};
    MotorGroupInstance_t yawMotors = {&rudderMotorInstance, 1};
    MotorGroupInstance_t throttleMotors = {&throttleMotorInstance, 1};

    flightMode = new DirectMapping();
    amHandle = new AttitudeManager(amRCQueueHandle, flightMode, rollMotors, pitchMotors, yawMotors, throttleMotors);
}
