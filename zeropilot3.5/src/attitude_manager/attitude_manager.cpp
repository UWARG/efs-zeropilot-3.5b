//Need to update this cpp later
#include "attitude_manager.hpp"
#include "rc_motor_control.hpp"
#include "direct_mapping.hpp"

AttitudeManagerInput AttitudeManager::am_control_inputs = { 0.0f, 0.0f, 0.0f, 0.0f};


RCMotorControlMessage_t AttitudeManager::getControlInputs() {

    RCMotorControlMessage_t sm_control_inputs = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}; //Now it holds arm data as well

    // Get data from Queue
    if (queue_driver->count()==0) {
        //Why are we returning RCMotorControlMessage_t? Can we convert it to AM's control inputs struct?
         return { sm_control_inputs.roll, sm_control_inputs.pitch, sm_control_inputs.yaw, sm_control_inputs.throttle, sm_control_inputs.arm };
    }
    sm_control_inputs = queue_driver->get();
    queue_driver->pop();
    
    return sm_control_inputs;
}

AttitudeManager::AttitudeManager(Flightmode* controlAlgorithm,  MotorGroup_t rollMotors, MotorGroup_t pitchMotors, MotorGroup_t yawMotors, MotorGroup_t throttleMotors, IMessageQueue<RCMotorControlMessage_t> *queue_driver)
:
    controlAlgorithm_(controlAlgorithm),
    rollMotors_(rollMotors),
    pitchMotors_(pitchMotors),
    yawMotors_(yawMotors),
    throttleMotors_(throttleMotors)
{}   


AttitudeManager::~AttitudeManager()
{}

void AttitudeManager::runControlLoopIteration() {
    // Get data from Queue and motor outputs
    RCMotorControlMessage_t control_inputs = getControlInputs();

    if (control_inputs.roll == 0 && control_inputs.pitch == 0 && control_inputs.yaw == 0 && control_inputs.throttle == -1) {
        // Do something
    }

    AttitudeManagerInput motor_outputs = controlAlgorithm_->run_control(control_inputs); // This is a placeholder for the actual control algorithm

    outputToMotor(yaw, motor_outputs.yaw);
    outputToMotor(pitch, motor_outputs.pitch);
    outputToMotor(roll, motor_outputs.roll);
    outputToMotor(throttle, motor_outputs.throttle);
}



void AttitudeManager::outputToMotor(ControlAxis_e axis, uint8_t percent) {
    MotorGroup_t* motorGroup = nullptr;

    switch (axis) {
        case roll:
            motorGroup = &rollMotors_; //those &Motors is a motor group with multiple motors movements- details undecided
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

