#include "museq.hpp"
#include "rc_motor_control.hpp"
#include "config_msg.hpp"

/* --- mutexes --- */
/* define mutexes begin */
osMutexId_t itmMutex;
osMessageQueueId_t amQueueId;
osMessageQueueId_t smLoggerQueueId;
osMessageQueueId_t smConfigQueueId;
osMessageQueueId_t smConfigAttitudeQueueId;

static const osMutexAttr_t itmMutexAttr = {
  "itmMutex",
  osMutexPrioInherit,
  NULL,
  0U
};
/* define mutexes end */

void initMutexes()
{
  itmMutex = osMutexNew(&itmMutexAttr);
}

/* --- sempahores --- */
/* define semaphores begin */
/* define semaphores end */

void initSemphrs()
{

}

/* --- queues --- */
/* define queues begin */
/* define queues end */

void initQueues()
{
  amQueueId = osMessageQueueNew(16, sizeof(RCMotorControlMessage_t), NULL);
  smLoggerQueueId = osMessageQueueNew(16, sizeof(char[100]), NULL);
  smConfigQueueId = osMessageQueueNew(16, sizeof(ConfigMessage_t), NULL);
  smConfigAttitudeQueueId = osMessageQueueNew(16, sizeof(ConfigMessage_t), NULL);
}
