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
	int read() override;
	GPS(UART_HandleTypeDef *huart);


private:
	UART_HandleTypeDef *huart;

	gps_data_t gps_data;
	uint8_t rx_buffer[MAX_NMEA_DATA_LENGTH];

	int parseRMC();
	int parseGGA();
};
