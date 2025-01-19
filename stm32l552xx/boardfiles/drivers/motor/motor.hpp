#pragma once

#include <cstdint>
#include "main.h"
#include "motor_iface.hpp"

class MotorControl {
    public:
		MotorControl(TIM_HandleTypeDef* timer, uint16_t timerChannel);

        void set(uint8_t percent);
    private:
        TIM_HandleTypeDef *timer;
        const uint16_t TIMER_CHANNEL;

    	// set to minimum and max amount of counts in a duty cycle
        const uint32_t MIN_SIGNAL;
        const uint32_t MAX_SIGNAL;
        const uint16_t DESIRED_FREQUENCY = 50; // PWM frequency in hz
};


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
