#include "motor.hpp"

MotorControl::MotorControl(TIM_HandleTypeDef* timer, uint16_t timerChannel) : timer(timer), TIMER_CHANNEL(timerChannel),MAX_SIGNAL(0), MIN_SIGNAL(0)
{
    // Calculate new pre-scaler
    uint16_t prescaler = (SystemCoreClock / DESIRED_FREQUENCY / timer->Init.Period) - 1;
    __HAL_TIM_SET_PRESCALER(timer, prescaler);

    // Set base value to minimum duty cycle and start PWM
    __HAL_TIM_SET_COMPARE(timer, TIMER_CHANNEL, MIN_SIGNAL);
    HAL_TIM_PWM_Start(timer, TIMER_CHANNEL);
}


bool MotorControl::setDutyCycleLimits(uint8_t minPercentage, uint8_t maxPercentage) {
    if (minPercentage > maxPercentage || minPercentage < 0 || maxPercentage > 100) {
        return false;
    }

    MIN_SIGNAL = static_cast<uint32_t>((minPercentage / 100.0) * timer->Init.Period);
    MAX_SIGNAL = static_cast<uint32_t>((maxPercentage / 100.0) * timer->Init.Period);

    // Set base value to new minimum duty cycle
    __HAL_TIM_SET_COMPARE(timer, TIMER_CHANNEL, MIN_SIGNAL);

    return true;
}

void MotorControl::start() {
	TIM1->CCR1 = (MIN_SIGNAL / 100.0) * TIM1->ARR;
	HAL_TIM_PWM_Start(timer, TIMER_CHANNEL);
}

void MotorControl::set(uint8_t percent) {
    if (percent > 100) {
        return;
    }

    uint32_t ticks = ((percent / 100.0) * (MAX_SIGNAL - MIN_SIGNAL)) + MIN_SIGNAL;

    __HAL_TIM_SET_COMPARE(timer, TIMER_CHANNEL, ticks);
}
