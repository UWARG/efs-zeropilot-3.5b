#include "drivers.hpp"
#include "museq.hpp"
#include "stm32l5xx_hal.h"
#include "error.h"

extern IWDG_HandleTypeDef hiwdg;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart4;

SystemUtils *systemUtilsHandle = nullptr;

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

ZP_ERROR_e initDrivers()
{
    systemUtilsHandle = new SystemUtils();
    if (systemUtilsHandle == nullptr) {
      return ZP_ERROR_OUT_OF_MEMORY;
    }

    iwdgHandle = new IndependentWatchdog(&hiwdg);
    if (iwdgHandle == nullptr) {
      return ZP_ERROR_OUT_OF_MEMORY;
    }

    loggerHandle = new Logger(); // Initialized in a RTOS task
    if (loggerHandle == nullptr) {
      return ZP_ERROR_OUT_OF_MEMORY;
    }

    leftAileronMotorHandle = new MotorControl(&htim3, TIM_CHANNEL_1, 5, 10);
    if (leftAileronMotorHandle == nullptr) {
      return ZP_ERROR_OUT_OF_MEMORY;
    }

    rightAileronMotorHandle = new MotorControl(&htim3, TIM_CHANNEL_2, 5, 10);
    if (rightAileronMotorHandle == nullptr) {
      return ZP_ERROR_OUT_OF_MEMORY;
    }

    elevatorMotorHandle = new MotorControl(&htim3, TIM_CHANNEL_3, 5, 10);
    if (elevatorMotorHandle == nullptr) {
      return ZP_ERROR_OUT_OF_MEMORY;
    }

    rudderMotorHandle = new MotorControl(&htim3, TIM_CHANNEL_4, 5, 10);
    if (rudderMotorHandle == nullptr) {
      return ZP_ERROR_OUT_OF_MEMORY;
    }

    throttleMotorHandle = new MotorControl(&htim4, TIM_CHANNEL_1, 5, 10);
    if (throttleMotorHandle == nullptr) {
      return ZP_ERROR_OUT_OF_MEMORY;
    }

    leftFlapMotorHandle = new MotorControl(&htim1, TIM_CHANNEL_1, 5, 10);
    if (leftFlapMotorHandle == nullptr) {
      return ZP_ERROR_OUT_OF_MEMORY;
    }

    rightFlapMotorHandle = new MotorControl(&htim1, TIM_CHANNEL_2, 5, 10);
    if (rightFlapMotorHandle == nullptr) {
      return ZP_ERROR_OUT_OF_MEMORY;
    }

    steeringMotorHandle = new MotorControl(&htim1, TIM_CHANNEL_3, 5, 10);
    if (steeringMotorHandle == nullptr) {
      return ZP_ERROR_OUT_OF_MEMORY;
    }

    gpsHandle = new GPS(&huart2);
    if (gpsHandle == nullptr) {
      return ZP_ERROR_OUT_OF_MEMORY;
    }

    rcHandle = new RCReceiver(&huart4);
    if (rcHandle == nullptr) {
      return ZP_ERROR_OUT_OF_MEMORY;
    }

    rfdHandle = new RFD(&huart3);
    if (rfdHandle == nullptr) {
      return ZP_ERROR_OUT_OF_MEMORY;
    }

    amRCQueueHandle = new MessageQueue<RCMotorControlMessage_t>(&amQueueId);
    if (amRCQueueHandle == nullptr) {
      return ZP_ERROR_OUT_OF_MEMORY;
    }

    smLoggerQueueHandle = new MessageQueue<char[100]>(&smLoggerQueueId);
    if (smLoggerQueueHandle == nullptr) {
      return ZP_ERROR_OUT_OF_MEMORY;
    }

    tmQueueHandle = new MessageQueue<TMMessage_t>(&tmQueueId);
    if (tmQueueHandle == nullptr) {
      return ZP_ERROR_OUT_OF_MEMORY;
    }

    messageBufferHandle = new MessageQueue<mavlink_message_t>(&messageBufferId);
    if (messageBufferHandle == nullptr) {
      return ZP_ERROR_OUT_OF_MEMORY;
    }

    leftAileronMotorHandle->init();
    rightAileronMotorHandle->init();
    elevatorMotorHandle->init();
    rudderMotorHandle->init();
    throttleMotorHandle->init();
    leftFlapMotorHandle->init();
    rightFlapMotorHandle->init();
    steeringMotorHandle->init();

    rcHandle->init();
    gpsHandle->init();

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

    return ZP_ERROR_OK;
}
