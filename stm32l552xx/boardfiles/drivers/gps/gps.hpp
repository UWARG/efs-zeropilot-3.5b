#pragma once

#include <stm32l5xx_hal_uart.h>
#include "gps_iface.hpp"
#include "gps_datatypes.hpp"
#include "gps_defines.hpp"

#ifdef __cplusplus
#include <cstdint>
#endif


class GPS : public IGPS {

public:
	gpsData_t readData() override;

	GPS(UART_HandleTypeDef *huart);

	int init();
	int processGPSData();


private:
	gpsData_t data;
	uint8_t rxBuffer[MAX_NMEA_DATA_LENGTH];
	UART_HandleTypeDef *huart;

	int parseRMC();
	int parseGGA();

};
