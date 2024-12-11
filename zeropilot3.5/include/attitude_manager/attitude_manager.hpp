#ifndef ZPSW3AM_HPP
#define ZPSW3_AM_HPP

#include "CommonDataTypes.hpp"
#include "config_foundation.hpp"
#include "FreeRTOS.h"
#include "flightmode.hpp"
#include "semphr.h"
#include "queue_iface.hpp"
#include "rc_motor_control.hpp"

namespace AM {

typedef struct {
    MotorChannel motorInstance; 
    bool isInverted;
} MotorInstance_t;

struct MotorGroup_t {
    MotorInstance_tmotors;
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

    Flightmode *controlAlgorithm;
    MotorGroup_t rollMotors;
    MotorGroup_t pitchMotors;
    MotorGroup_t yawMotors;
    MotorGroup_t throttleMotors;

    IMessageQueue *queue_driver;
};

}  // namespace AM

#endif  // ZPSW3_AM_HPP