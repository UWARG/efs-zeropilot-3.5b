//
// Created by Anthony Luo on 2022-10-12.
//
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

    // Write "motor_outputs" from previous step to motors
    outputToMotor(yaw, static_cast<uint8_t>(motor_outputs.yaw));
    outputToMotor(pitch, static_cast<uint8_t>(motor_outputs.pitch));
    outputToMotor(roll, static_cast<uint8_t>(motor_outputs.roll));
    outputToMotor(throttle, static_cast<uint8_t>(motor_outputs.throttle));

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