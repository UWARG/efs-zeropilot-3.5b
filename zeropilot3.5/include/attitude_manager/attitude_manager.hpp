#pragma once

#include <cstdint>
#include "systemutils_iface.hpp"
#include "flightmode.hpp"
#include "motor_datatype.hpp"
#include "gps_iface.hpp"
#include "tm_queue.hpp"
#include "queue_iface.hpp"

#define AM_CONTROL_LOOP_DELAY 50
#define AM_FAILSAFE_TIMEOUT 1000

typedef enum {
    YAW = 0,
    PITCH,
    ROLL,
    THROTTLE,
    FLAP_ANGLE,
    STEERING
} ControlAxis_t;

class AttitudeManager {
    public:
        AttitudeManager(
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
        );

        void runControlLoopIteration();

    private:
        ISystemUtils *systemUtilsDriver;

        IGPS *gpsDriver;

        IMessageQueue<RCMotorControlMessage_t> *amQueue;
        IMessageQueue<TMMessage_t> *tmQueue;
        IMessageQueue<char[100]> *smLoggerQueue;

        Flightmode *controlAlgorithm;
        RCMotorControlMessage_t controlMsg;

        MotorGroupInstance_t *rollMotors;
        MotorGroupInstance_t *pitchMotors;
        MotorGroupInstance_t *yawMotors;
        MotorGroupInstance_t *throttleMotors;
        MotorGroupInstance_t *flapMotors;
        MotorGroupInstance_t *steeringMotors;

        bool previouslyArmed;
        float armAltitude;

        uint8_t amSchedulingCounter;

        bool getControlInputs(RCMotorControlMessage_t *pControlMsg);

        void outputToMotor(ControlAxis_t axis, uint8_t percent);

        void sendGPSDataToTelemetryManager(const GpsData_t &gpsData, const bool &armed);
};
