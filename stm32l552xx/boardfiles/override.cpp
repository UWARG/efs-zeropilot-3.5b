#include "main.h"
#include "museq.hpp"

extern "C"
{
/* overriding _write to redirect puts()/printf() to SWO */
int _write(int file, char *ptr, int len)
{
    if (osMutexAcquire(itmMutex, osWaitForever) == osOK) {
        for (int DataIdx = 0; DataIdx < len; DataIdx++) {
            ITM_SendChar(ptr[DataIdx]);
        }
        osMutexRelease(itmMutex);
    }
    return len;
}

/* interrupt callback functions */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    // GPS dma callback
    if (&huart2 == huart) {
      gps->processGPSData();
    }
}
}
