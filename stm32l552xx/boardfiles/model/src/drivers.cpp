#include "drivers.hpp"

extern UART_HandleTypeDef huart4;

RCReceiver *rcHandle = nullptr;
GPS *gpsHandle = nullptr;

void initDrivers()
{
    rcHandle = new RCReceiver(&huart4); 
    gpsHandle = new GPS(&huart2);
}
