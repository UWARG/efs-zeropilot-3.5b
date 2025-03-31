#pragma once

#include <cstdint>
#include "flightmode.hpp"
#include "queue_iface.hpp"
#include "motor_iface.hpp"
#include "motor_datatype.hpp"

typedef enum {
    YAW = 0,
    PITCH,
    ROLL,
    THROTTLE
} ControlAxis_e;

class AttitudeManager {
    public:
        AttitudeManager(
            IMessageQueue<RCMotorControlMessage_t> *amQueue, 
            Flightmode *controlAlgorithm,  
            MotorGroupInstance_t rollMotors, 
            MotorGroupInstance_t pitchMotors, 
            MotorGroupInstance_t yawMotors, 
            MotorGroupInstance_t throttleMotors
        );

        void runControlLoopIteration();

    private:
        IMessageQueue<RCMotorControlMessage_t> *amQueue;

        Flightmode *controlAlgorithm;
        RCMotorControlMessage_t controlMsg;

        MotorGroupInstance_t rollMotors;
        MotorGroupInstance_t pitchMotors;
        MotorGroupInstance_t yawMotors;
        MotorGroupInstance_t throttleMotors;

        bool getControlInputs(RCMotorControlMessage_t *pControlMsg);

        void outputToMotor(ControlAxis_e axis, uint8_t percent);
        void disarmToMotor(RCMotorControlMessage_t controlMsg);
};
