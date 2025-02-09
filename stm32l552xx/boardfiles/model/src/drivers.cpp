#include "drivers.hpp"

extern UART_HandleTypeDef huart4;

IRCReceiver *rcHandle = nullptr;

void initDrivers()
{
    rcHandle = new RCReceiver(&huart4); 


}
