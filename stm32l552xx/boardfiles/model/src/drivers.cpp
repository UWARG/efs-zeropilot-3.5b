#include "drivers.hpp"
#include "iwdg.hpp"
#include "logger.hpp"
#include "motor.hpp"
#include "rc.hpp"
#include "stm32l5xx_hal.h"


extern IWDG_HandleTypeDef hiwdg;
extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart4;

IndependentWatchdog *iwdgHandle = nullptr;
Logger *loggerHandle = nullptr;
MotorControl *motorHandle = nullptr;
RCReceiver *rcHandle = nullptr;

void initDrivers()
{
    iwdgHandle = new IndependentWatchdog(&hiwdg);
    loggerHandle = new Logger();
    motorHandle = new MotorControl(&htim2, TIMER_CHANNEL_1, 5, 10);
    rcHandle = new RCReceiver(&huart4);
}
