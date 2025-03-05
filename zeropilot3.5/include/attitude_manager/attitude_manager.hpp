#ifndef ZPSW3AM_HPP
#define ZPSW3_AM_HPP

#include "flightmode.hpp"
#include "queue_iface.hpp"
#include "motor_iface.hpp"
#include "rc_motor_control.hpp"
#include <stdint.h>

typedef struct {
    IMotorControl *motorInstance; 
    bool isInverted;
} MotorInstance_t;

typedef struct {
    float roll;
    float pitch;
    float yaw;
    float throttle;
} AttitudeManagerInput;

struct MotorGroup_t {
    MotorInstance_t motors;
    uint8_t motorCount;
} MotorGroupInstance_t;

class AttitudeManager {
   public:
    // Constants used for mapping values
    static constexpr float INPUT_MAX = 100;
    static constexpr float INPUT_MIN = -100;

    //Are we going to use <T> or a definite variabe type? Using a template here causes imcomplete data type issue in cpp
    AttitudeManager(Flightmode* controlAlgorithm,  MotorGroup_t rollMotors, MotorGroup_t pitchMotors, MotorGroup_t yawMotors, MotorGroup_t throttleMotors, IMessageQueue<RCMotorControlMessage_t> *queue_driver);
    ~AttitudeManager();

    void runControlLoopIteration();

   private:

    enum ControlAxis_e {
        yaw,
        pitch,
        roll,
        throttle
    };
    
    static AttitudeManagerInput am_control_inputs;
    RCMotorControlMessage_t getControlInputs();

    AttitudeManager();
    void outputToMotor(ControlAxis_e axis, uint8_t percent);
    //What should go into template? AttitudeManagerInput?
    Flightmode*controlAlgorithm_;
    MotorGroup_t rollMotors;
    MotorGroup_t pitchMotors;
    MotorGroup_t yawMotors;
    MotorGroup_t throttleMotors;
    IMessageQueue<RCMotorControlMessage_t> *queue_driver;
};

#endif  // ZPSW3_AM_HPP