#include "am_threads.hpp"
#include "managers.hpp"
#include "utils.h"
#include "logger.hpp"

osThreadId_t amMainHandle;
osThreadId_t startUp;
extern Logger * loggerHandle;

static const osThreadAttr_t amMainLoopAttr = {
    .name = "amMain",
    .stack_size = 1024,
    .priority = (osPriority_t) osPriorityNormal
};
static const osThreadAttr_t startupTask_attributes = {
    .name = "startUp",
    .stack_size = 1024,
    .priority = (osPriority_t) osPriorityHigh
};

void amMainLoopWrapper(void *arg)
{
  while(true)
  {
    amHandle->runControlLoopIteration();
    osDelay(timeToTicks(50));
  }
}

void StartupTask(void *argument) {
  loggerHandle->init();  // NOW safe - kernel is running
  vTaskDelete(NULL);     // Task done, delete itself
}
void amInitThreads()
{
    startUp = osThreadNew(StartupTask, NULL, &startupTask_attributes);
    amMainHandle = osThreadNew(amMainLoopWrapper, NULL, &amMainLoopAttr);
}

