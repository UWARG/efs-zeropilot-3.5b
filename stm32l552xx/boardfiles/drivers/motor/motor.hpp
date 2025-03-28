#pragma once

#include "motor_iface.hpp"
#include "stm32l5xx_hal.h"

class MotorControl : public IMotorControl {
    public:
        MotorControl(TIM_HandleTypeDef *timer, uint32_t timerChannel, uint32_t minDutyCycle, uint32_t maxDutyCycle);
        
        void set(uint32_t percent) override;

        void init();

    private:
        TIM_HandleTypeDef * const timer;
        const uint32_t timerChannel;
        const uint32_t minCCR;
        const uint32_t maxCCR;
};
