#pragma once

#include "iwdg.hpp"
#include "logger.hpp"
#include "motor.hpp"
#include "motor_datatype.hpp"
#include "rc.hpp"
#include "rc_motor_control.hpp"
#include "config_msg.hpp"
#include "SDIO.hpp"
#include "queue.hpp"

extern IndependentWatchdog *iwdgHandle;
extern SDIO *textIOHandle;
extern SDIO *configTextIOHandle;

extern MotorControl *leftAileronMotorHandle;
extern MotorControl *rightAileronMotorHandle;
extern MotorControl *elevatorMotorHandle;
extern MotorControl *rudderMotorHandle;
extern MotorControl *throttleMotorHandle;
extern MotorControl *leftFlapMotorHandle;
extern MotorControl *rightFlapMotorHandle;
extern MotorControl *steeringMotorHandle;

extern RCReceiver *rcHandle;

extern MessageQueue<RCMotorControlMessage_t> *amRCQueueHandle;
extern MessageQueue<char[100]> *smLoggerQueueHandle;
extern MessageQueue<ConfigMessage_t> *smConfigQueueHandle;
extern MessageQueue<ConfigMessage_t> *smConfigAttitudeQueueHandle;

extern MotorGroupInstance_t rollMotors;
extern MotorGroupInstance_t pitchMotors;
extern MotorGroupInstance_t yawMotors;
extern MotorGroupInstance_t throttleMotors;
extern MotorGroupInstance_t flapMotors;
extern MotorGroupInstance_t steeringMotors;

void initDrivers();
