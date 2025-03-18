#include "drivers.hpp"

extern UART_HandleTypeDef huart4;

RCReceiver *rcHandle = nullptr;

void initDrivers()
{
    rcHandle = new RCReceiver(&huart4); 
}
