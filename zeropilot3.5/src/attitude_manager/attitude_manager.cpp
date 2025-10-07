#include "attitude_manager.hpp"
#include "rc_motor_control.hpp"

#define AM_SCHEDULING_RATE_HZ 20
#define AM_TELEMETRY_GPS_DATA_RATE_HZ 5

AttitudeManager::AttitudeManager(
    ISystemUtils *systemUtilsDriver,
    IGPS *gpsDriver,
    IMessageQueue<RCMotorControlMessage_t> *amQueue,
    IMessageQueue<TMMessage_t> *tmQueue,
    IMessageQueue<char[100]> *smLoggerQueue,
    Flightmode *controlAlgorithm,
    MotorGroupInstance_t *rollMotors,
    MotorGroupInstance_t *pitchMotors,
    MotorGroupInstance_t *yawMotors,
    MotorGroupInstance_t *throttleMotors,
    MotorGroupInstance_t *flapMotors,
    MotorGroupInstance_t *steeringMotors
) :
    systemUtilsDriver(systemUtilsDriver),
    gpsDriver(gpsDriver),
    amQueue(amQueue),
    tmQueue(tmQueue),
    smLoggerQueue(smLoggerQueue),
    controlAlgorithm(controlAlgorithm),
    rollMotors(rollMotors),
    pitchMotors(pitchMotors),
    yawMotors(yawMotors),
    throttleMotors(throttleMotors),
    flapMotors(flapMotors),
    steeringMotors(steeringMotors),
    previouslyArmed(false),
    armAltitude(0.0f),
    amSchedulingCounter(0) {}

void AttitudeManager::runControlLoopIteration() {
    // Get data from Queue and motor outputs
    const bool CONTROL_RES = getControlInputs(&controlMsg);
    
    // Failsafe
    static bool failsafeTriggered = false;

    if (CONTROL_RES != true) {
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

    const RCMotorControlMessage_t MOTOR_OUTPUTS = controlAlgorithm->runControl(controlMsg);

    outputToMotor(YAW, MOTOR_OUTPUTS.yaw);
    outputToMotor(PITCH, MOTOR_OUTPUTS.pitch);
    outputToMotor(ROLL, MOTOR_OUTPUTS.roll);
    outputToMotor(THROTTLE, MOTOR_OUTPUTS.throttle);
    outputToMotor(FLAP_ANGLE, MOTOR_OUTPUTS.flapAngle);
    outputToMotor(STEERING, MOTOR_OUTPUTS.yaw);

    // Send GPS data to telemetry manager
    const GpsData_t GPS_DATA = gpsDriver->readData();
    if (amSchedulingCounter % (AM_SCHEDULING_RATE_HZ / AM_TELEMETRY_GPS_DATA_RATE_HZ) == 0) {
        sendGPSDataToTelemetryManager(GPS_DATA, controlMsg.arm > 0);
    }

    amSchedulingCounter = (amSchedulingCounter + 1) % AM_SCHEDULING_RATE_HZ;
}

bool AttitudeManager::getControlInputs(RCMotorControlMessage_t *pControlMsg) const {
    if (amQueue->count() == 0) {
        return false;
    }

    amQueue->get(pControlMsg);
    return true;
}

void AttitudeManager::outputToMotor(const ControlAxis_t AXIS, const uint8_t PERCENT) const {
    const MotorGroupInstance_t *motorGroup = nullptr;

    switch (AXIS) {
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
    }

    for (uint8_t i = 0; i < motorGroup->motorCount; i++) {
        const MotorInstance_t *motor = motorGroup->motors + i;

        if (motor->isInverted) {
            motor->motorInstance->set(100 - PERCENT);
        }
        else {
            motor->motorInstance->set(PERCENT);
        }
    }
}


void AttitudeManager::sendGPSDataToTelemetryManager(const GpsData_t &gpsData, const bool &armed) {
    if (!gpsData.isNew) return;

    if (armed) {
        if (!previouslyArmed) {
            armAltitude = gpsData.altitude;
            previouslyArmed = true;
        }
    } else {
        previouslyArmed = false;
        armAltitude = 0.0f;
    }

    // calculate relative altitude
    const float RELATIVE_ALTITUDE = previouslyArmed ? gpsData.altitude - armAltitude : 0.0f;

    TMMessage_t gpsDataMsg = gposDataPack(
        systemUtilsDriver->getCurrentTimestampMs(), // time_boot_ms
        gpsData.altitude * 1000, // altitude in mm
        gpsData.latitude * 1e7,
        gpsData.longitude * 1e7,
        RELATIVE_ALTITUDE * 1000, // relative altitude in mm
        gpsData.vx,
        gpsData.vy,
        gpsData.vz,
        gpsData.trackAngle
    );

    tmQueue->push(&gpsDataMsg);
}
