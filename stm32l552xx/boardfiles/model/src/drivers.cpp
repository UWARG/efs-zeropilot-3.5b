#include "drivers.hpp"
#include "museq.hpp"
#include "stm32l5xx_hal.h"

//extern IWDG_HandleTypeDef hiwdg;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern UART_HandleTypeDef huart4;

//IndependentWatchdog *iwdgHandle = nullptr;
Logger *loggerHandle = nullptr;

MotorControl *leftAileronMotorHandle = nullptr;
MotorControl *rightAileronMotorHandle = nullptr;
MotorControl *elevatorMotorHandle = nullptr;
MotorControl *rudderMotorHandle = nullptr;
MotorControl *throttleMotorHandle = nullptr;

RCReceiver *rcHandle = nullptr;

MessageQueue<RCMotorControlMessage_t> *amRCQueueHandle = nullptr;
MessageQueue<char[100]> *smLoggerQueueHandle = nullptr;

MotorInstance_t rollLeftMotorInstance;
MotorInstance_t rollRightMotorInstance;
MotorInstance_t elevatorMotorInstance;
MotorInstance_t rudderMotorInstance;
MotorInstance_t throttleMotorInstance;

MotorInstance_t rollMotorInstance[2];

MotorGroupInstance_t rollMotors;
MotorGroupInstance_t pitchMotors;
MotorGroupInstance_t yawMotors;
MotorGroupInstance_t throttleMotors;

void initDrivers()
{
    //iwdgHandle = new IndependentWatchdog(&hiwdg);
    loggerHandle = new Logger();

    leftAileronMotorHandle = new MotorControl(&htim3, TIM_CHANNEL_1, 5, 10);
    rightAileronMotorHandle = new MotorControl(&htim3, TIM_CHANNEL_2, 5, 10);
    elevatorMotorHandle = new MotorControl(&htim3, TIM_CHANNEL_3, 5, 10);
    rudderMotorHandle = new MotorControl(&htim3, TIM_CHANNEL_4, 5, 10);
    throttleMotorHandle = new MotorControl(&htim4, TIM_CHANNEL_1, 5, 10);
    
    rcHandle = new RCReceiver(&huart4);

    amRCQueueHandle = new MessageQueue<RCMotorControlMessage_t>(&amQueueId);
    smLoggerQueueHandle = new MessageQueue<char[100]>(&smLoggerQueueId);

    loggerHandle->init();

    leftAileronMotorHandle->init();
    rightAileronMotorHandle->init();
    elevatorMotorHandle->init();
    rudderMotorHandle->init();
    throttleMotorHandle->init();

    rcHandle->init();

    rollLeftMotorInstance = {leftAileronMotorHandle, false};
    rollRightMotorInstance = {rightAileronMotorHandle, false};
    elevatorMotorInstance = {elevatorMotorHandle, false};
    rudderMotorInstance = {rudderMotorHandle, false};
    throttleMotorInstance = {throttleMotorHandle, false};

    rollMotorInstance[0] = rollLeftMotorInstance;
    rollMotorInstance[1] = rollRightMotorInstance;

    rollMotors = {rollMotorInstance, 2};
    pitchMotors = {&elevatorMotorInstance, 1};
    yawMotors = {&rudderMotorInstance, 1};
    throttleMotors = {&throttleMotorInstance, 1};
}
