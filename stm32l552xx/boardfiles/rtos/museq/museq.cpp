#include "museq.hpp"
#include "rc_motor_control.hpp"
#include "tm_queue.hpp"
#include "mavlink.h"

/* --- mutexes --- */
/* define mutexes begin */
osMutexId_t itmMutex;
osMutexId_t canBroadcastMutex;

osMessageQueueId_t amQueueId;
osMessageQueueId_t smLoggerQueueId;
osMessageQueueId_t tmQueueId;
osMessageQueueId_t messageBufferId;

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
  canBroadcastMutex = osMutexNew(&itmMutexAttr);

  if (itmMutex == NULL || canBroadcastMutex == NULL){
    // need to handle failure
  }
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
  tmQueueId = osMessageQueueNew(16, sizeof(TMMessage_t), NULL);
  messageBufferId = osMessageQueueNew(16, sizeof(mavlink_message_t), NULL);
}
