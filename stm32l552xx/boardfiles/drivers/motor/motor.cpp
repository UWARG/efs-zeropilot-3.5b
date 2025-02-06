#include "motor.hpp"

MotorControl::MotorControl(TIM_HandleTypeDef* timer, uint16_t timerChannel) : timer(timer), TIMER_CHANNEL(timerChannel), MIN_SIGNAL(timer->Init.Period * 0.05), MAX_SIGNAL(timer->Init.Period * 0.10)  // sets counts for 5% duty cycleMAX_SIGNAL(timer->Init.Period * 0.10)  // sets counts for 10% duty cycle
{
	HAL_TIM_PWM_Start(timer, TIMER_CHANNEL);
}

void MotorControl::set(uint8_t percent) {
    if (percent > 100) {
        return;
    }

    uint32_t ticks = ((percent / 100.0) * (MAX_SIGNAL - MIN_SIGNAL)) + MIN_SIGNAL;

    __HAL_TIM_SET_COMPARE(timer, TIMER_CHANNEL, ticks);
}
