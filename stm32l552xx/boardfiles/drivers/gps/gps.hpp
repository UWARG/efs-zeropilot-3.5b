#pragma once

#include "stm32l5xx_hal.h"
#include "gps_iface.hpp"
#include "gps_defines.hpp"
#include <cmath>

class GPS : public IGPS {

public:
    GpsData_t readData() override;

    GPS(UART_HandleTypeDef *huart);

    bool init();
    bool processGPSData();
    uint8_t rxBuffer[MAX_NMEA_DATA_LENGTH];

private:
    GpsData_t validData;
    GpsData_t tempData;
    
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
};
