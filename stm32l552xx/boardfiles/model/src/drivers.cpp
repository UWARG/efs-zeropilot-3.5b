#include "drivers.hpp"
#include "museq.hpp"
#include "stm32l5xx_hal.h"

extern IWDG_HandleTypeDef hiwdg;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern UART_HandleTypeDef huart4;

IndependentWatchdog *iwdgHandle = nullptr;
Logger *loggerHandle = nullptr;

MotorControl *leftAileronMotorHandle = nullptr;
MotorControl *rightAileronMotorHandle = nullptr;
MotorControl *elevatorMotorHandle = nullptr;
MotorControl *rudderMotorHandle = nullptr;
MotorControl *throttleMotorHandle = nullptr;

RCReceiver *rcHandle = nullptr;

MessageQueue<RCMotorControlMessage_t> *amRCQueueHandle = nullptr;
MessageQueue<char[100]> *smLoggerQueueHandle = nullptr;

void initDrivers()
{
    iwdgHandle = new IndependentWatchdog(&hiwdg);
    loggerHandle = new Logger();

    leftAileronMotorHandle = new MotorControl(&htim3, TIM_CHANNEL_1, 5, 10);
    rightAileronMotorHandle = new MotorControl(&htim3, TIM_CHANNEL_2, 5, 10);
    elevatorMotorHandle = new MotorControl(&htim3, TIM_CHANNEL_3, 5, 10);
    rudderMotorHandle = new MotorControl(&htim3, TIM_CHANNEL_4, 5, 10);
    throttleMotorHandle = new MotorControl(&htim4, TIM_CHANNEL_1, 5, 10);
    
    rcHandle = new RCReceiver(&huart4);

    amRCQueueHandle = new MessageQueue<RCMotorControlMessage_t>(&amQueueId);
    smLoggerQueueHandle = new MessageQueue<char[100]>(&smLoggerQueueId);
}
