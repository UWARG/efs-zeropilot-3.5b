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
};

class AttitudeManager {
   public:
    // Constants used for mapping values
    static constexpr float INPUT_MAX = 100;
    static constexpr float INPUT_MIN = -100;

    static AttitudeManagerInput getControlInputs();
    
    AttitudeManager(Flightmode* controlAlgorithm,  MotorGroup_t rollMotors, MotorGroup_t pitchMotors, MotorGroup_t yawMotors, MotorGroup_t throttleMotors, IMessageQueue *queue_driver);
    ~AttitudeManager();

    void runControlLoopIteration();

   private:
    AttitudeManager();
    void outputToMotor(ControlAxis_t axis, uint8_t percent);

    static struct AttitudeManagerInput control_inputs;

    Flightmode *controlAlgorithm_;
    MotorGroup_t rollMotors;
    MotorGroup_t pitchMotors;
    MotorGroup_t yawMotors;
    MotorGroup_t throttleMotors;

    IMessageQueue *queue_driver;
};

#endif  // ZPSW3_AM_HPP