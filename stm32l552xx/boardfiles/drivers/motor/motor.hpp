#pragma once

#include "motor_iface.hpp"
#include "main.h"

class MotorControl{
    public:
        MotorControl(TIM_HandleTypeDef* timer, uint16_t timerChannel);

        /**
         * @brief Sets the duty cycle as a percent between 5 and 10%
         * @param percent: number between 1-100
         */
        void set(uint8_t percent);
        bool setDutyCycleLimits(uint8_t minPercentage, uint8_t maxPercentage);
        void start();
    private:
        TIM_HandleTypeDef *const timer;
        const uint16_t TIMER_CHANNEL;

    	// set to minimum and max amount of counts in a duty cycle
        uint32_t MIN_SIGNAL;
        uint32_t MAX_SIGNAL;
        const uint16_t DESIRED_FREQUENCY = 50;
};
