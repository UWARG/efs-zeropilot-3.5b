#include "motor.hpp"

MotorControl::MotorControl(TIM_HandleTypeDef* timer, uint16_t timerChannel, uint32_t min_percent, uint32_t max_percent) : timer(timer), TIMER_CHANNEL(timerChannel), MIN_SIGNAL(timer->Init.Period * min_percent/100.0), MAX_SIGNAL(timer->Init.Period * max_percent/100.0){}
void MotorControl::init()
{
    // Set base value to minimum duty cycle and start PWM
    __HAL_TIM_SET_COMPARE(timer, TIMER_CHANNEL, MIN_SIGNAL);
    HAL_TIM_PWM_Start(timer, TIMER_CHANNEL);
}


void MotorControl::set(uint8_t percent) {
    if (percent > 100) {
        return;
    }

    uint32_t ticks = ((percent / 100.0) * (MAX_SIGNAL - MIN_SIGNAL)) + MIN_SIGNAL;

    __HAL_TIM_SET_COMPARE(timer, TIMER_CHANNEL, ticks);
}
