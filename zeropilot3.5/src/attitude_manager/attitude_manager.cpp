//Need to update this cpp later
#include "attitude_manager.hpp"
#include "rc_motor_control.hpp"

//This possibly belongs to AM
AttitudeManagerInput AttitudeManager::control_inputs = { 0.0f, 0.0f, 0.0f, 0.0f};


AttitudeManagerInput AttitudeManager::getControlInputs() {
    //This looks from SM
    RCMotorControlMessage_t control_inputs = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}; //Now it holds arm data as well

    // Get data from Queue
    if (queue_driver->count()==0) {
        //Why are we returning RCMotorControlMessage_t? Can we convert it to AM's control inputs struct?
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

    RCMotorControlMessage_t motor_outputs = controlAlgorithm_->run(control_inputs); // This is a placeholder for the actual control algorithm

    outputToMotor(yaw, motor_outputs.yaw);
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
        if (motorGroup->motors[motorCount].isInverted) {
            motorGroup->motors[motorCount].motorInstance->set(100 - percent);
        } else {
            motorGroup->motors[motorCount].motorInstance->set(percent);
        }
    }

}

