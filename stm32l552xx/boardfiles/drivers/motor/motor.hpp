#pragma once

#include <cstdint>
#include "main.h"
#include "motor_iface.hpp"

class MotorControl : IMotorControl {
    public:
		MotorControl(TIM_HandleTypeDef* timer, uint16_t timerChannel);

        /**
         * @brief Sets the duty cycle as a percent between 5 and 10%
         * @param percent: number between 1-100
         */
        void set(uint8_t percent);
        
    private:
        TIM_HandleTypeDef *timer;
        const uint16_t TIMER_CHANNEL;

    	// set to minimum and max amount of counts in a duty cycle
        const uint32_t MIN_SIGNAL;
        const uint32_t MAX_SIGNAL;
};