#pragma once

#include "systemutils.hpp"
#include "iwdg.hpp"
#include "logger.hpp"
#include "motor.hpp"
#include "motor_datatype.hpp"
#include "rc.hpp"
#include "rc_motor_control.hpp"
#include "system_manager/config_utils/config_keys.hpp"
#include "SDIO.hpp"
#include "tm_queue.hpp"
#include "mavlink.h"
#include "queue.hpp"
#include "gps.hpp"
#include "rfd.hpp"

extern SystemUtils *systemUtilsHandle;

extern IndependentWatchdog *iwdgHandle;
extern SDIO *textIOHandle;

extern MotorControl *leftAileronMotorHandle;
extern MotorControl *rightAileronMotorHandle;
extern MotorControl *elevatorMotorHandle;
extern MotorControl *rudderMotorHandle;
extern MotorControl *throttleMotorHandle;
extern MotorControl *leftFlapMotorHandle;
extern MotorControl *rightFlapMotorHandle;
extern MotorControl *steeringMotorHandle;

extern RCReceiver *rcHandle;
extern GPS *gpsHandle;

extern RFD *rfdHandle;

extern MessageQueue<RCMotorControlMessage_t> *amRCQueueHandle;
extern MessageQueue<char[100]> *smLoggerQueueHandle;
extern MessageQueue<ConfigMessage_t> *smConfigAttitudeQueueHandle;
extern MessageQueue<ConfigMessage_t> *smConfigRouteQueueHandle[static_cast<size_t>(Owner::COUNT)];
extern MessageQueue<TMMessage_t> *tmQueueHandle;
extern MessageQueue<TMSMMessage_t> *tmSmQueueHandle;
extern MessageQueue<mavlink_message_t> *messageBufferHandle;

extern MotorGroupInstance_t rollMotors;
extern MotorGroupInstance_t pitchMotors;
extern MotorGroupInstance_t yawMotors;
extern MotorGroupInstance_t throttleMotors;
extern MotorGroupInstance_t flapMotors;
extern MotorGroupInstance_t steeringMotors;

void initDrivers();
