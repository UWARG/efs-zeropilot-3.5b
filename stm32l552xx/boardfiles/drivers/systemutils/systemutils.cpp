#include "cmsis_os.h"
#include "systemutils.hpp"
#include "stm32l5xx_hal.h"

void SystemUtils::delay(uint32_t delay_ms) {
    HAL_Delay(delay_ms);
}

uint32_t SystemUtils::getCurrentTimestamp() {
    return osKernelGetTickCount() / osKernelGetTickFreq();
}