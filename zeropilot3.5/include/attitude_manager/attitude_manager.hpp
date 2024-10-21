/*
 * AM.hpp
 *
 * Attitude Manager Header
 *
 * Created on: Oct 12, 2022
 * Author(s): Anthony (anni) Luo; Dhruv Upadhyay
 */
#ifndef ZPSW3_AM_HPP
#define ZPSW3_AM_HPP

#include "CommonDataTypes.hpp"
#include "config_foundation.hpp"
#include "FreeRTOS.h"
#include "flightmode.hpp"
#include "semphr.h"
#ifdef TESTING
#include <gtest/gtest_prod.h>
#endif

namespace AM {

typedef struct {
    MotorChannel *motorInstance;
    bool isInverted;
} MotorInstance_t;

struct MotorGroup_t {
    MotorInstance_t *motors;
    uint8_t motorCount;
}

class AttitudeManager {
   public:

    // Constants used for mapping values
    static constexpr float INPUT_MAX = 100;
    static constexpr float INPUT_MIN = -100;

    static void setControlInputs(const AttitudeManagerInput& new_control_inputs);

    static AttitudeManagerInput getControlInputs();

    AttitudeManager(Flightmode* controlAlgorithm,  MotorGroup_t rollMotors, MotorGroup_t pitchMotors, MotorGroup_t yawMotors, MotorGroup_t throttleMotors);

    ~AttitudeManager();

    void runControlLoopIteration();

   private:
    #ifdef TESTING
    FRIEND_TEST(AttitudeManager, MotorInitializationAndOutput); // Remove FRIEND_TEST when updating tests with setControlInputs
    FRIEND_TEST(AttitudeManagerOutputToMotor, NoMotors);
    FRIEND_TEST(AttitudeManagerOutputToMotor, MotorsOfSameAxis);
    FRIEND_TEST(AttitudeManagerOutputToMotor, setOutputInRandomAxisOrder);
    FRIEND_TEST(AttitudeManagerOutputToMotor, InvertedTest);
    FRIEND_TEST(AttitudeManagerOutputToMotor, CombinedTest);
    #endif

    AttitudeManager();
    void outputToMotor(ControlAxis_t axis, uint8_t percent);

    static SemaphoreHandle_t control_inputs_mutex;
    static struct AttitudeManagerInput control_inputs;

    Flightmode *controlAlgorithm_;
    MotorGroup_t rollMotors_;
    MotorGroup_t pitchMotors_;
    MotorGroup_t yawMotors_;
    MotorGroup_t throttleMotors_;

};

}  // namespace AM

#endif  // ZPSW3_AM_HPP