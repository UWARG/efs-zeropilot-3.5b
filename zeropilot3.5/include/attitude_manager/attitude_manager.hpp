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
    //I guess controlaxis_T is another leftover from the previous design, I'll look up what it served for.
    void outputToMotor(ControlAxis_t axis, uint8_t percent);

    static struct AttitudeManagerInput control_inputs;

    //what data type would you give to the flightmode/Imessagequeue? Would you like me to define or would you want to discuss about it
    //Or already has it in mind, I just didn't see it. (We can use the previous one if you want, too.)

    Flightmode *controlAlgorithm_;
    MotorGroup_t rollMotors;
    MotorGroup_t pitchMotors;
    MotorGroup_t yawMotors;
    MotorGroup_t throttleMotors;

    IMessageQueue *queue_driver;
};

#endif  // ZPSW3_AM_HPP