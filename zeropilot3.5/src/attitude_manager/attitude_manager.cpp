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

ZP_ERROR_e AttitudeManager::runControlLoopIteration() {
    // Get data from Queue and motor outputs
    ZP_ERROR_e controlRes = getControlInputs(&controlMsg);

    // Failsafe
    static bool failsafeTriggered = false;

    if (controlRes != ZP_ERROR_OK) {
        ++noDataCount;

        if (noDataCount * AM_MAIN_DELAY > 1000) {
            ZP_ERROR_e err;
            err = outputToMotor(YAW, 50);
            if (err != ZP_ERROR_OK) return err;

            err = outputToMotor(PITCH, 50);
            if (err != ZP_ERROR_OK) return err;

            err = outputToMotor(ROLL, 50);
            if (err != ZP_ERROR_OK) return err;

            err = outputToMotor(THROTTLE, 0);
            if (err != ZP_ERROR_OK) return err;

            err = outputToMotor(FLAP_ANGLE, 0);
            if (err != ZP_ERROR_OK) return err;

            err = outputToMotor(STEERING, 50);
            if (err != ZP_ERROR_OK) return err;

            if (!failsafeTriggered) {
              char errorMsg[100] = "Failsafe triggered";
              err = smLoggerQueue->push(&errorMsg);
              if (err != ZP_ERROR_OK) return err;
              failsafeTriggered = true;
            }
        }

        return ZP_ERROR_OK;
    } else {
        noDataCount = 0;

        if (failsafeTriggered) {
          char errorMsg[100] = "Motor control restored";
          ZP_ERROR_e err = smLoggerQueue->push(&errorMsg);
          if (err != ZP_ERROR_OK) return err;
          failsafeTriggered = false;
        }
    }

    // Disarm
    if (controlMsg.arm == 0) {
        controlMsg.throttle = 0;
    }

    RCMotorControlMessage_t motorOutputs;
    ZP_ERROR_e err = controlAlgorithm->runControl(&motorOutputs, controlMsg);
    if (err != ZP_ERROR_OK) return err;

    err = outputToMotor(YAW, motorOutputs.yaw);
    if (err != ZP_ERROR_OK) return err;

    err = outputToMotor(PITCH, motorOutputs.pitch);
    if (err != ZP_ERROR_OK) return err;

    err = outputToMotor(ROLL, motorOutputs.roll);
    if (err != ZP_ERROR_OK) return err;

    err = outputToMotor(THROTTLE, motorOutputs.throttle);
    if (err != ZP_ERROR_OK) return err;

    err = outputToMotor(FLAP_ANGLE, motorOutputs.flapAngle);
    if (err != ZP_ERROR_OK) return err;

    err = outputToMotor(STEERING, motorOutputs.yaw);
    if (err != ZP_ERROR_OK) return err;

    // Send GPS data to telemetry manager
    GpsData_t gpsData;
    err = gpsDriver->readData(&gpsData);
    if (err != ZP_ERROR_OK) return err;

    if (amSchedulingCounter % (AM_SCHEDULING_RATE_HZ / AM_TELEMETRY_GPS_DATA_RATE_HZ) == 0) {
        err = sendGPSDataToTelemetryManager(gpsData, controlMsg.arm > 0);
        if (err != ZP_ERROR_OK) return err;
    }

    amSchedulingCounter = (amSchedulingCounter + 1) % AM_SCHEDULING_RATE_HZ;

    return ZP_ERROR_OK;
}

ZP_ERROR_e AttitudeManager::getControlInputs(RCMotorControlMessage_t *pControlMsg) {
    int count = 0;
    ZP_ERROR_e err = amQueue->count(&count);
    if (err != ZP_ERROR_OK) {
        return err;
    }

    if (count == 0) {
        return ZP_ERROR_RESOURCE_UNAVAILABLE;
    }

    err = amQueue->get(pControlMsg);
    if (err != ZP_ERROR_OK) {
        return err;
    }

    return ZP_ERROR_OK;
}

ZP_ERROR_e AttitudeManager::outputToMotor(ControlAxis_t axis, uint8_t percent) {
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
            return ZP_ERROR_INVALID_PARAM;
    }

    for (uint8_t i = 0; i < motorGroup->motorCount; i++) {
        MotorInstance_t *motor = (motorGroup->motors + i);
        ZP_ERROR_e err;

        if (motor->isInverted) {
            err = motor->motorInstance->set(100 - percent);
        }
        else {
            err = motor->motorInstance->set(percent);
        }

        if (err != ZP_ERROR_OK) {
            return err;
        }
    }

    return ZP_ERROR_OK;
}


ZP_ERROR_e AttitudeManager::sendGPSDataToTelemetryManager(const GpsData_t &gpsData, const bool &armed) {
    if (!gpsData.isNew) return ZP_ERROR_OK;

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
    float relativeAltitude = previouslyArmed ? (gpsData.altitude - armAltitude) : 0.0f;

    uint32_t timestampMs = 0;
    ZP_ERROR_e err = systemUtilsDriver->getCurrentTimestampMs(&timestampMs);
    if (err != ZP_ERROR_OK) {
        return err;
    }

    TMMessage_t gpsDataMsg = gposDataPack(
        timestampMs, // time_boot_ms
        gpsData.altitude * 1000, // altitude in mm
        gpsData.latitude * 1e7,
        gpsData.longitude * 1e7,
        relativeAltitude * 1000, // relative altitude in mm
        gpsData.vx,
        gpsData.vy,
        gpsData.vz,
        gpsData.trackAngle
    );

    err = tmQueue->push(&gpsDataMsg);
    if (err != ZP_ERROR_OK) {
        return err;
    }

    return ZP_ERROR_OK;
}
