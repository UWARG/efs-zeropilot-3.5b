#include "drivers.hpp"
#include "museq.hpp"
#include "stm32l5xx_hal.h"

extern IWDG_HandleTypeDef hiwdg;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart4;

IndependentWatchdog *iwdgHandle = nullptr;
Logger *loggerHandle = nullptr;

MotorControl *leftAileronMotorHandle = nullptr;
MotorControl *rightAileronMotorHandle = nullptr;
MotorControl *elevatorMotorHandle = nullptr;
MotorControl *rudderMotorHandle = nullptr;
MotorControl *throttleMotorHandle = nullptr;
MotorControl *leftFlapMotorHandle = nullptr;
MotorControl *rightFlapMotorHandle = nullptr;
MotorControl *steeringMotorHandle = nullptr;

GPS *gpsHandle = nullptr;
RCReceiver *rcHandle = nullptr;

RFD *rfdHandle = nullptr;

MessageQueue<RCMotorControlMessage_t> *amRCQueueHandle = nullptr;
MessageQueue<char[100]> *smLoggerQueueHandle = nullptr;
MessageQueue<TMMessage_t> *tmQueueHandle = nullptr;
MessageQueue<mavlink_message_t> *messageBufferHandle = nullptr;

MotorInstance_t rollLeftMotorInstance;
MotorInstance_t rollRightMotorInstance;
MotorInstance_t elevatorMotorInstance;
MotorInstance_t rudderMotorInstance;
MotorInstance_t throttleMotorInstance;
MotorInstance_t leftFlapMotorInstance;
MotorInstance_t rightFlapMotorInstance;
MotorInstance_t steeringMotorInstance;

MotorInstance_t rollMotorInstance[2];
MotorInstance_t flapMotorInstance[2];

MotorGroupInstance_t rollMotors;
MotorGroupInstance_t pitchMotors;
MotorGroupInstance_t yawMotors;
MotorGroupInstance_t throttleMotors;
MotorGroupInstance_t flapMotors;
MotorGroupInstance_t steeringMotors;

void initDrivers()
{
    iwdgHandle = new IndependentWatchdog(&hiwdg);
    loggerHandle = new Logger();

    leftAileronMotorHandle = new MotorControl(&htim3, TIM_CHANNEL_1, 5, 10);
    rightAileronMotorHandle = new MotorControl(&htim3, TIM_CHANNEL_2, 5, 10);
    elevatorMotorHandle = new MotorControl(&htim3, TIM_CHANNEL_3, 5, 10);
    rudderMotorHandle = new MotorControl(&htim3, TIM_CHANNEL_4, 5, 10);
    throttleMotorHandle = new MotorControl(&htim4, TIM_CHANNEL_1, 5, 10);
    leftFlapMotorHandle = new MotorControl(&htim1, TIM_CHANNEL_1, 5, 10);
    rightFlapMotorHandle = new MotorControl(&htim1, TIM_CHANNEL_2, 5, 10);
    steeringMotorHandle = new MotorControl(&htim1, TIM_CHANNEL_3, 5, 10);
    
    gpsHandle = new GPS(&huart2);
    rcHandle = new RCReceiver(&huart4);

    rfdHandle = new RFD(&huart3);

    amRCQueueHandle = new MessageQueue<RCMotorControlMessage_t>(&amQueueId);
    smLoggerQueueHandle = new MessageQueue<char[100]>(&smLoggerQueueId);
    tmQueueHandle = new MessageQueue<TMMessage_t>(&tmQueueId);
    messageBufferHandle = new MessageQueue<mavlink_message_t>(&messageBufferId);
    loggerHandle->init();

    leftAileronMotorHandle->init();
    rightAileronMotorHandle->init();
    elevatorMotorHandle->init();
    rudderMotorHandle->init();
    throttleMotorHandle->init();
    leftFlapMotorHandle->init();
    rightFlapMotorHandle->init();
    steeringMotorHandle->init();

    rcHandle->init();

    rollLeftMotorInstance = {leftAileronMotorHandle, true};
    rollRightMotorInstance = {rightAileronMotorHandle, true};
    elevatorMotorInstance = {elevatorMotorHandle, false};
    rudderMotorInstance = {rudderMotorHandle, false};
    throttleMotorInstance = {throttleMotorHandle, false};
    leftFlapMotorInstance = {leftFlapMotorHandle, false};
    rightFlapMotorInstance = {rightFlapMotorHandle, true};
    steeringMotorInstance = {steeringMotorHandle, true};

    rollMotorInstance[0] = rollLeftMotorInstance;
    rollMotorInstance[1] = rollRightMotorInstance;

    flapMotorInstance[0] = leftFlapMotorInstance;
    flapMotorInstance[1] = rightFlapMotorInstance;

    rollMotors = {rollMotorInstance, 2};
    pitchMotors = {&elevatorMotorInstance, 1};
    yawMotors = {&rudderMotorInstance, 1};
    throttleMotors = {&throttleMotorInstance, 1};
    flapMotors = {flapMotorInstance, 2};
    steeringMotors = {&steeringMotorInstance, 1};
}
