#include "am_threads.hpp"
#include "managers.hpp"
#include "utils.h"

osThreadId_t amMainHandle;

static const osThreadAttr_t amMainLoopAttr = {
    "amMain",
    osThreadDetached,
    nullptr,
    0U,
    nullptr,
    0U,
    osPriorityNormal,
    0U,
    0U
};

void amMainLoopWrapper(void *arg)
{
  while(true)
  {
    amHandle->runControlLoopIteration();
    osDelay(timeToTicks(1000));
  }
}

void amInitThreads()
{
    amMainHandle = osThreadNew(amMainLoopWrapper, nullptr, &amMainLoopAttr);
}
