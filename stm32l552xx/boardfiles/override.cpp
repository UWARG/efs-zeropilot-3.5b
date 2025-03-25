#include "cmsis_os2.h"
#include "main.h"
#include "museq.hpp"
#include "utils.h"

extern "C"
{
/* overriding _write to redirect puts()/printf() to SWO */
int _write(int file, char *ptr, int len)
{
  if( osMutexAcquire(itmMutex, osWaitForever) == osOK )
  {
    for (int DataIdx = 0; DataIdx < len; DataIdx++)
    {
      ITM_SendChar(ptr[DataIdx]);
    }
    osMutexRelease(itmMutex);
  }
  return len;
}

void HAL_Delay(uint32_t Delay) {
  if (osKernelGetState() == osKernelRunning) {
    osDelayUntil(osKernelGetTickCount() + timeToTicks(Delay));
  } else {
    uint32_t tickstart = HAL_GetTick();
    uint32_t wait = Delay;

    if (wait < HAL_MAX_DELAY) {
      wait += (uint32_t)uwTickFreq;
    }

    while ((HAL_GetTick() - tickstart) < wait) {}
  }
}

/* interrupt callback functions */
}
