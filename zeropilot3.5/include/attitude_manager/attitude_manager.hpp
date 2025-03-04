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
    AttitudeManager(Flightmode<AttitudeManagerInput>* controlAlgorithm,  MotorGroup_t rollMotors, MotorGroup_t pitchMotors, MotorGroup_t yawMotors, MotorGroup_t throttleMotors, IMessageQueue<int> *queue_driver);
    ~AttitudeManager();

    void runControlLoopIteration();

   private:
    AttitudeManager();
  
    void outputToMotor(ControlAxis_t axis, uint8_t percent);

    static struct AttitudeManagerInput control_inputs;
    static struct AttitudeManagerInput getControlInputs();

    //What should go into template? AttitudeManagerInput?
    Flightmode<AttitudeManagerInput> *controlAlgorithm_;
    MotorGroup_t rollMotors;
    MotorGroup_t pitchMotors;
    MotorGroup_t yawMotors;
    MotorGroup_t throttleMotors;
    IMessageQueue<int> *queue_driver;
};

#endif  // ZPSW3_AM_HPP