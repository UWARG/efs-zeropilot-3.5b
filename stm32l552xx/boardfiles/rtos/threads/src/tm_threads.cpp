#include "tm_threads.hpp"
#include "managers.hpp"
#include "utils.h"

osThreadId_t tmMainHandle;

static const osThreadAttr_t tmMainLoopAttr = {
    .name = "tmMain",
    .stack_size = 1024,
    .priority = (osPriority_t) osPriorityNormal
};

void tmMainLoopWrapper(void *arg)
{
  while(true)
  {

	tmHandle->heartBeatMsg();
	tmHandle->transmit();
	osDelay(timeToTicks(10));

	tmHandle->gpsMsg();
	tmHandle->transmit();
	osDelay(timeToTicks(10));

	tmHandle->processMsgQueue();
	tmHandle->transmit();
	osDelay(timeToTicks(10));
  }
}

void tmInitThreads()
{
    tmMainHandle = osThreadNew(tmMainLoopWrapper, NULL, &tmMainLoopAttr);
}
