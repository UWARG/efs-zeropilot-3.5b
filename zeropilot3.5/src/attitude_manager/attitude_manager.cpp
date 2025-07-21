#include "attitude_manager.hpp"
#include "rc_motor_control.hpp"
#include <cstdio>


AttitudeManager::AttitudeManager(
    IMessageQueue<RCMotorControlMessage_t> *amQueue,
    IMessageQueue<char[100]> *smLoggerQueue,
    Flightmode *controlAlgorithm,
    MotorGroupInstance_t *rollMotors,
    MotorGroupInstance_t *pitchMotors,
    MotorGroupInstance_t *yawMotors,
    MotorGroupInstance_t *throttleMotors,
    MotorGroupInstance_t *flapMotors,
    MotorGroupInstance_t *steeringMotors
) :
    amQueue(amQueue),
    smLoggerQueue(smLoggerQueue),
    controlAlgorithm(controlAlgorithm),
    rollMotors(rollMotors),
    pitchMotors(pitchMotors),
    yawMotors(yawMotors),
    throttleMotors(throttleMotors),
    flapMotors(flapMotors),
    steeringMotors(steeringMotors) {}

void AttitudeManager::setRudderMixing(float coeff) {
    if (coeff < 0.0f || coeff > 1.0f) {
        return;
    }

    adverseCoeff = coeff;
    signedYaw = 0.0f; // Reset signedYaw to avoid incorrect values
    
}
void AttitudeManager::runControlLoopIteration() {
    // Get data from Queue and motor outputs
    bool res = getControlInputs(&controlMsg);

    // Failsafe
    static bool failsafeTriggered = false;

    if (res != true) {
        ++noDataCount;

        if (noDataCount * AM_MAIN_DELAY > 1000) {
            outputToMotor(YAW, 50);
            outputToMotor(PITCH, 50);
            outputToMotor(ROLL, 50);
            outputToMotor(THROTTLE, 0);
            outputToMotor(FLAP_ANGLE, 0);
            outputToMotor(STEERING, 50);

            if (!failsafeTriggered) {
              char errorMsg[100] = "Failsafe triggered";
              smLoggerQueue->push(&errorMsg);
              failsafeTriggered = true;
            }
        }

        return;
    } else {
        noDataCount = 0;

        if (failsafeTriggered) {
          char errorMsg[100] = "Motor control restored";
          smLoggerQueue->push(&errorMsg);
          failsafeTriggered = false;
        }
    }

    // Disarm
    if (controlMsg.arm == 0) {
        controlMsg.throttle = 0;
    }

    RCMotorControlMessage_t motorOutputs = controlAlgorithm->runControl(controlMsg);

    signedYaw = motorOutputs.roll-50;
    adverseYaw = signedYaw * adverseCoeff;
    motorOutputs.yaw +=adverseYaw; 
    // limit yaw to 100
    if (motorOutputs.yaw>100){
        motorOutputs.yaw = 100;
    //limit yaw to 0
    } else if (motorOutputs.yaw < 0) {
        motorOutputs.yaw = 0;
    }
    
    outputToMotor(YAW, (100-motorOutputs.yaw)*if_yawMotors_invert+motorOutputs.yaw*(if_yawMotors_invert-1)+YAWMOTORS_TRIM);
    outputToMotor(PITCH, (100-motorOutputs.pitch)*if_pitchMotors_invert+motorOutputs.pitch*(if_pitchMotors_invert-1)+PITCHMOTORS_TRIM);
    outputToMotor(ROLL, (100-motorOutputs.roll)*if_rollMotors_invert+motorOutputs.roll*(if_rollMotors_invert-1)+ROLLMOTORS_TRIM);
    outputToMotor(THROTTLE, (100-motorOutputs.throttle)*if_throttleMotors_invert+motorOutputs.throttle*(if_throttleMotors_invert-1)+THROTTLEMOTORS_TRIM);
    outputToMotor(FLAP_ANGLE, (100-motorOutputs.flapAngle)*if_flapMotors_invert+motorOutputs.flapAngle*(if_flapMotors_invert-1)+FLAPMOTORS_TRIM);
    outputToMotor(STEERING, motorOutputs.yaw+STEERINGMOTORS_TRIM);
}

bool AttitudeManager::getControlInputs(RCMotorControlMessage_t *pControlMsg) {
    if (amQueue->count() == 0) {
        return false;
    }

    amQueue->get(pControlMsg);
    return true;
}

void AttitudeManager::outputToMotor(ControlAxis_t axis, uint8_t percent) {
    MotorGroupInstance_t *motorGroup = nullptr;

    switch (axis) {
        case ROLL:
            motorGroup = rollMotors;
            break;
        case PITCH:
            motorGroup = pitchMotors;
            break;
        case YAW:
            motorGroup = yawMotors;
            break;
        case THROTTLE:
            motorGroup = throttleMotors;
            break;
        case FLAP_ANGLE:
            motorGroup = flapMotors;
            break;
        case STEERING:
            motorGroup = steeringMotors;
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

