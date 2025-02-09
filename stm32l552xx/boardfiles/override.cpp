#include "main.h"
#include "museq.hpp"
#include "rc.hpp"

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

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == UART4){
<<<<<<< Updated upstream
		// insert driver handle
    parse(true);
=======
		rcHandle->parse(0); //modify, true/false
>>>>>>> Stashed changes
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == UART4){
<<<<<<< Updated upstream
    // insert driver handle
		parse(false);
=======
		rcHandle->parse(1); //modify; true/false
>>>>>>> Stashed changes
	}
}

}
