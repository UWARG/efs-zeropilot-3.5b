#ifndef MOTOR_HPP_
#define MOTOR_HPP_

#include "tim.h"

#include "motor_iface.hpp"

class MotorChannel : public MotorControl {
    public:
        MotorChannel(TIM_HandleTypeDef* timer,
					uint16_t timerChannel);

        void set(uint_8 percent) override;
    private:
        TIM_HandleTypeDef *timer;
        const uint16_t TIMER_CHANNEL;

    	// set to minimum and max amount of counts in a duty cycle
        const uint32_t MIN_SIGNAL;
        const uint32_t MAX_SIGNAL;
        const uint16_t DESIRED_FREQUENCY = 50; // PWM frequency in hz
};

#endif

/*
#ifndef MOTOR_CONTROL_HPP
#define MOTOR_CONTROL_HPP


class MotorControl {
public:
    virtual ~MotorControl() = default;

    // Set the motor speed
    virtual void set(uint8_t percent) = 0;
protected:
    MotorControl() = default;
};

#endif // MOTOR_CONTROL_HPP
*/
