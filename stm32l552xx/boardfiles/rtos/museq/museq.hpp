#pragma once

#include "cmsis_os.h"

/* --- mutexes --- */
/* declare mutexes begin*/
extern osMutexId_t itmMutex;
/* declare mutexes end*/

void initMutexes();

/* --- semaphores --- */
/* declare semaphores begin */
/* declare semaphores end */

void initSemphrs();

/* --- queues --- */
/* declare queues begin */
extern osMessageQueueId_t amQueueId;
extern osMessageQueueId_t smLoggerQueueId;
/* declare queues end */

void initQueues();
