#include "sm_threads.hpp"
#include "managers.hpp"
#include "utils.h"

osThreadId_t smMainHandle;

static const osThreadAttr_t smMainLoopAttr = {
    "smMain",
    osThreadDetached,
    nullptr,
    0U,
    nullptr,
    0U,
    osPriorityNormal,
    0U,
    0U
};

void smMainLoopWrapper(void *arg)
{
  while(true)
  {
    smHandle->SMUpdate();
    osDelay(timeToTicks(1000));
  }
}

void smInitThreads()
{
    smMainHandle = osThreadNew(smMainLoopWrapper, nullptr, &smMainLoopAttr);
}
