#include "drivers.hpp"

extern FDCAN_HandleTypeDef hfdcan1;

CAN *canHandle = nullptr;

void initDrivers()
{
    canHandle = new CAN(&hfdcan1);
    if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK) {
  		Error_Handler();
  	}
}
