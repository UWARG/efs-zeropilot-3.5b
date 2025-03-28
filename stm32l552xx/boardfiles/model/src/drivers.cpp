#include "drivers.hpp"

extern UART_HandleTypeDef huart4;

RCReceiver *rcHandle = nullptr;

#include "iwdg.hpp"
#include "logger.hpp"
#include "motor.hpp"

#include "stm32l5xx_hal.h"

extern IWDG_HandleTypeDef hiwdg;
extern TIM_HandleTypeDef htim2;

IndependentWatchdog *iwdgHandle = nullptr;
Logger *loggerHandle = nullptr;
MotorControl *motorHandle = nullptr;

void initDrivers()
{
    rcHandle = new RCReceiver(&huart4);     iwdgHandle = new IndependentWatchdog(&hiwdg);
    loggerHandle = new Logger();
    motorHandle = new MotorControl(&htim2, TIMER_CHANNEL_1, 5, 10);
}
