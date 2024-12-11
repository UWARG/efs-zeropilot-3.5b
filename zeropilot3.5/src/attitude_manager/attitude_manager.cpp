#include "AM.hpp"

#include "FreeRTOS.h"
#include "semphr.h"

namespace AM {

AttitudeManagerInput AttitudeManager::control_inputs = {
    .roll = 0.0f,
    .pitch = 0.0f,
    .yaw = 0.0f,
    .throttle = 0.0f
};

AttitudeManagerInput AttitudeManager::getControlInputs() {
    RCMotorControlMessage_t control_inputs = {
        .roll = 0,
        .pitch = 0,
        .yaw = 0,
        .throttle = 0
    };

    // Get data from Queue
    if (queue_driver->count()==0) {
        return control_inputs;
    }
    control_inputs = queue_driver->get();
    queue_driver->pop();
    return control_inputs;
}

AttitudeManager::AttitudeManager(Flightmode* controlAlgorithm, MotorGroup_t rollMotors, MotorGroup_t pitchMotors, MotorGroup_t yawMotors, MotorGroup_t throttleMotors) :
    controlAlgorithm_(controlAlgorithm),
    rollMotors_(rollMotors),
    pitchMotors_(pitchMotors),
    yawMotors_(yawMotors),
    throttleMotors_(throttleMotors)
{};

AttitudeManager::~AttitudeManager()
{}

void AttitudeManager::runControlLoopIteration() {
    // Get data from Queue and motor outputs
    RCMotorControlMessage_t control_inputs = getControlInputs();

    if (control_inputs.roll == 0 && control_inputs.pitch == 0 && control_inputs.yaw == 0 && control_inputs.throttle == -1) {
        // Do something
    }
    outputToMotor(yaw, control_inputs.yaw);
    outputToMotor(pitch, motor_outputs.pitch);
    outputToMotor(roll, motor_outputs.roll);
    outputToMotor(throttle, motor_outputs.throttle);

}

void AttitudeManager::outputToMotor(ControlAxis_t axis, uint8_t percent) {
    MotorGroup_t* motorGroup = nullptr;

    switch (axis) {
        case roll:
            motorGroup = &rollMotors_;
            break;
        case pitch:
            motorGroup = &pitchMotors_;
            break;
        case yaw:
            motorGroup = &yawMotors_;
            break;
        case throttle:
            motorGroup = &throttleMotors_;
            break;
        default:
            return;
    }

    for (uint8_t count = 0; count < motorGroup->motorCount; count++) {
        if (motorGroup_t->motors[motorCount].isInverted) {
            motorGroup->motors[motorCount].motorInstance->set(100 - percent);
        } else {
            motorGroup->motors[motorCount].motorInstance->set(percent);
        }
    }

}

}  // namespace AM
