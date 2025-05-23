#include "attitude_manager.hpp"

AttitudeManager::AttitudeManager(
    IMessageQueue<RCMotorControlMessage_t> *amQueue, 
    Flightmode *controlAlgorithm,  
    MotorGroupInstance_t rollMotors, 
    MotorGroupInstance_t pitchMotors, 
    MotorGroupInstance_t yawMotors, 
    MotorGroupInstance_t throttleMotors
) : 
    amQueue(amQueue),
    controlAlgorithm(controlAlgorithm),
    rollMotors(rollMotors),
    pitchMotors(pitchMotors),
    yawMotors(yawMotors),
    throttleMotors(throttleMotors) {} 

void AttitudeManager::runControlLoopIteration() {
    // Get data from Queue and motor outputs
    bool res = getControlInputs(&controlMsg);

    // Failsafe
    if (res != true) {
        ++noDataCount;

        if (noDataCount * AM_MAIN_DELAY > 500) {
            outputToMotor(YAW, 50);
            outputToMotor(PITCH, 50);
            outputToMotor(ROLL, 50);
            outputToMotor(THROTTLE, 0);
        }

        return;
    }
    else {
        noDataCount = 0;
    }

    // Disarm
    if (controlMsg.arm == 0) {
        controlMsg.throttle = 0;
    }

    RCMotorControlMessage_t motorOutputs = controlAlgorithm->runControl(controlMsg);

    outputToMotor(YAW, motorOutputs.yaw);
    outputToMotor(PITCH, motorOutputs.pitch);
    outputToMotor(ROLL, motorOutputs.roll);
    outputToMotor(THROTTLE, motorOutputs.throttle);
}

bool AttitudeManager::getControlInputs(RCMotorControlMessage_t *pControlMsg) {
    if (amQueue->count() == 0) {
        return false;
    }

    *pControlMsg = amQueue->get();
    amQueue->pop();
    return true;
}

void AttitudeManager::outputToMotor(ControlAxis_e axis, uint8_t percent) {
    MotorGroupInstance_t *motorGroup = nullptr;

    switch (axis) {
        case ROLL:
            motorGroup = &rollMotors;
            break;
        case PITCH:
            motorGroup = &pitchMotors;
            break;
        case YAW:
            motorGroup = &yawMotors;
            break;
        case THROTTLE:
            motorGroup = &throttleMotors;
            break;
        default:
            return;
    }

    for (uint8_t i = 0; i < motorGroup->motorCount; i++) {
        MotorInstance_t *motor = (motorGroup->motors + i);
        
        if (motor->isInverted) {
            motor->motorInstance->set(100 - percent);
        } 
        else {
            motor->motorInstance->set(percent);
        }
    }
}