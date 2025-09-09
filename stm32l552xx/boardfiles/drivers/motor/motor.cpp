#include "motor.hpp"
#include "error.hpp"

MotorControl::MotorControl(TIM_HandleTypeDef *timer, uint32_t timerChannel, uint32_t minDutyCycle, uint32_t maxDutyCycle) : 
    timer(timer), 
    timerChannel(timerChannel), 
    minCCR(minDutyCycle / 100.0 * timer->Init.Period), 
    maxCCR(maxDutyCycle / 100.0 * timer->Init.Period) {
    // blank
}

ZP_ERROR_e MotorControl::set(uint32_t percent) {
    percent = percent > 100 ? 100 : percent;
    uint32_t ticks = ((percent / 100.0) * (maxCCR - minCCR)) + minCCR;
    __HAL_TIM_SET_COMPARE(timer, timerChannel, ticks);
    return ZP_ERROR_OK;
}

ZP_ERROR_e MotorControl::init() {
    __HAL_TIM_SET_COMPARE(timer, timerChannel, minCCR);
    HAL_TIM_PWM_Start(timer, timerChannel);
    return ZP_ERROR_OK;
}
