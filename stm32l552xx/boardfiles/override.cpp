#include "main.h"
#include "museq.hpp"
#include "rfd.hpp"

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

/* interrupt callback functions */

// make call back logic into own function then just call it here TODO
  void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size) {

    if (RFD::instance && RFD::instance->getHuart() == huart) {
      RFD::instance->receiveCallback(size);
    }
    
  }
}
