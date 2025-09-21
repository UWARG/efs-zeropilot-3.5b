#pragma once

#include <cstdint>
#include "flightmode.hpp"
#include "queue_iface.hpp"
#include "motor_iface.hpp"
#include "motor_datatype.hpp"

#define AM_MAIN_DELAY 50


typedef enum {
    YAW = 0,
    PITCH,
    ROLL,
    THROTTLE,
    FLAP_ANGLE,
    STEERING
} ControlAxis_e;

class AttitudeManager {
    public:
        AttitudeManager(
            IMessageQueue<RCMotorControlMessage_t> *amQueue,
            IMessageQueue<char[100]> *smLoggerQueue,
            Flightmode *controlAlgorithm,  
            MotorGroupInstance_t *rollMotors,
            MotorGroupInstance_t *pitchMotors,
            MotorGroupInstance_t *yawMotors,
            MotorGroupInstance_t *throttleMotors,
            MotorGroupInstance_t *flapMotors,
            MotorGroupInstance_t *steeringMotors
        );
        void setRudderMixing(float coeff);

        void runControlLoopIteration();

    private:
        IMessageQueue<RCMotorControlMessage_t> *amQueue;
        IMessageQueue<char[100]> *smLoggerQueue;

        Flightmode *controlAlgorithm;
        RCMotorControlMessage_t controlMsg;
        int noDataCount = 0;

        MotorGroupInstance_t *rollMotors;
        MotorGroupInstance_t *pitchMotors;
        MotorGroupInstance_t *yawMotors;
        MotorGroupInstance_t *throttleMotors;
        MotorGroupInstance_t *flapMotors;
        MotorGroupInstance_t *steeringMotors;
        
        
        float adverseCoeff=0.15f;
        float adverseYaw = 0.0f;
        float signedYaw = 0.0f;

        bool getControlInputs(RCMotorControlMessage_t *pControlMsg);

        void outputToMotor(ControlAxis_e axis, uint8_t percent);
};
