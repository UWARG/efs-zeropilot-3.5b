#pragma once

#include <stm32l5xx_hal_uart.h>
#include "gps_iface.hpp"
#include "gps_datatypes.hpp"
#include "gps_defines.hpp"
#include <cmath>

class GPS : public IGPS {

public:
    GpsData_t readData() override;

    GPS(UART_HandleTypeDef *huart);

    bool init();
    bool processGPSData();

private:
    GpsData_t validData;
    GpsData_t tempData;

    uint8_t rxBuffer[MAX_NMEA_DATA_LENGTH];
    UART_HandleTypeDef *huart;

    bool parseRMC();
    bool parseGGA();

    // RMC helper functions
    bool getTimeRMC(int &idx);
    bool getLatitudeRMC(int &idx);
    bool getLongitudeRMC(int &idx);
    bool getSpeedRMC(int &idx);
    bool getTrackAngleRMC(int &idx);
    bool getDateRMC(int &idx);


    // GGA helper functions
    bool getNumSatellitesGGA(int &idx);
    bool getAltitudeGGA(int &idx);
    bool getGeoidSeperationGGA(int &idx);
};
