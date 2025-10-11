#include "stm32l5xx_hal.h"

#include "gps.hpp"

GPS::GPS(UART_HandleTypeDef* huart) : huart(huart) {}

bool GPS::init() {
    HAL_StatusTypeDef success = HAL_UARTEx_ReceiveToIdle_DMA(
		huart,
		rxBuffer,
		MAX_NMEA_DATA_LENGTH
    );

    __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);

    HAL_StatusTypeDef messagesuccess = enableMessage(0x01, 0x11); //enable ubx velecef messages

    return (success == HAL_OK) & (messagesuccess == HAL_OK);
}

HAL_StatusTypeDef GPS::enableMessage(uint8_t msgClass, uint8_t msgId) {
    uint8_t cfgMsg[] = {
        0xB5, 0x62,         //sync chars
        0x06, 0x01,         //class and id
        0x03, 0x00,         //length
        msgClass, msgId,
        0x01,               //rate
        0x00, 0x00          //placeholder
    };

    if(sendUBX(cfgMsg, sizeof(cfgMsg))) {
        return HAL_OK;
    } else {
        return HAL_ERROR;
    }
}

void GPS::calcChecksum(uint8_t *msg, uint16_t len) {
    uint8_t ckA = 0, ckB = 0;
    for (int i = 2; i < len - 2; i++) {
        ckA += msg[i];
        ckB += ckA;
    }
    msg[len - 2] = ckA;
    msg[len - 1] = ckB;
}

bool GPS::sendUBX(uint8_t *msg, uint16_t len) {
    calcChecksum(msg, len);
    return (HAL_UART_Transmit(huart, msg, len, HAL_MAX_DELAY) == HAL_OK);
}

GpsData_t GPS::readData() {
    __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_TC);

    GpsData_t data = validData;
    validData.isNew = false;

    __HAL_DMA_ENABLE_IT(huart->hdmarx, DMA_IT_TC);

    return data;
}

bool GPS::processGPSData() {
    __HAL_DMA_DISABLE(huart->hdmarx);

    bool success = parseRMC() && parseGGA() && parseUBX();
    tempData.isNew = success;
    validData = tempData;

    __HAL_DMA_ENABLE(huart->hdmarx);

    return success;
}
//
bool GPS::parseUBX() {
    int idx = 0;

    // find sync
    while (!(rxBuffer[idx] == 0xB5 && rxBuffer[idx+1] == 0x62)) {
        idx++;
        if (idx >= MAX_NMEA_DATA_LENGTH) return false;  // not found
    }
    // check class
    if (rxBuffer[idx+2] != 0x01 || rxBuffer[idx+3] != 0x11) {
        return false;
    }

    // payload
    idx += 6;

    idx += 4;

    if(getVx(idx) == false) {
    	return false;
    }

    idx += 4;

    if(getVy(idx) == false) {
    	return false;
    }

    idx += 4;

    if (getVz(idx) == false) {
    	return false;
    }

    return true;
}


bool GPS::parseRMC() {
    int idx = 0;
    while (!(rxBuffer[idx] == 'R' && rxBuffer[idx+1] == 'M' && rxBuffer[idx+2] == 'C')) {
        idx++;
        if (idx == MAX_NMEA_DATA_LENGTH) return false;
    }

    idx += 4;

    // Check if data exists
    if (rxBuffer[idx] == ',') {
        return false;
    }

    if (getTimeRMC(idx) == false) {
        return false;
    }

    // Skip to status
    while (rxBuffer[idx] != ',') idx++;

    // Begin status
    idx++;

    // Check if data valid
    if (rxBuffer[idx] == 'V') return false;
    // End status

    idx += 2;

    if (getLatitudeRMC(idx) == false) {
        return false;
    }

    idx += 2;

    if (getLongitudeRMC(idx) == false) {
        return false;
    }

    idx += 2;

    if (getSpeedRMC(idx) == false) {
        return false;
    }

    while (rxBuffer[idx] != ',') idx++;
    idx++;

    if (getTrackAngleRMC(idx) == false) {
        return false;
    }

    while (rxBuffer[idx] != ',') idx++;
    while (rxBuffer[idx] == ',') idx++;

    if (getDateRMC(idx) == false) {
        return false;
    }

    return true;
}

bool GPS::parseGGA() {
    int idx = 0;
    while (!(rxBuffer[idx] == 'G' && rxBuffer[idx + 1] == 'G' && rxBuffer[idx + 2] == 'A')) {
        idx++;
        if (idx == MAX_NMEA_DATA_LENGTH) return false;
    }
    idx+=4; // Skip to data

    // Check if data exists
    if (rxBuffer[idx] == ',') {
        return false;
    }

    // Skip 7 sections of data
    for (int i = 0; i < 6; i++, idx++) {
        while (rxBuffer[idx] != ',') idx++;
    }

    if (getNumSatellitesGGA(idx) == false) {
        return false;
    }

    for (int i = 0; i < 2; i++, idx++) {
    	while (rxBuffer[idx] != ',') idx++;
    }

    if(getAltitudeGGA(idx) == false) {
    	return false;
    }

    return true;
}

bool GPS::getTimeRMC(int &idx) {
    uint8_t hour = (rxBuffer[idx] - '0') * 10 + (rxBuffer[idx + 1] - '0');
    idx += 2;
    uint8_t minute = (rxBuffer[idx] - '0') * 10 + (rxBuffer[idx + 1] - '0');
    idx += 2;
    uint8_t second = (rxBuffer[idx] - '0') * 10 + (rxBuffer[idx + 1] - '0');

    tempData.time.hour = hour;
    tempData.time.minute = minute;
    tempData.time.second = second;

    return true;
}

bool GPS::getLatitudeRMC(int &idx) {
    float lat = 0;
    for (int i = 0; i < 2; i++, idx++) {
        lat *= 10;
        lat += ((float)(rxBuffer[idx] - '0'));
    }

    float lat_minutes = 0;
    while (rxBuffer[idx] != '.') {
        lat_minutes *= 10;
        lat_minutes += ((float)(rxBuffer[idx] - '0'));
        idx++;
    }
    idx++; // Skip decimal char

    // Including two digits of minutes
    uint32_t mult = 10;
    while (rxBuffer[idx] != ',' && mult <= DECIMAL_PRECISION) {
        lat_minutes += ((float)(rxBuffer[idx] - '0')) / mult;
        idx++;
        mult *= 10;
    }

    lat += lat_minutes/60;

    tempData.latitude = lat;

    // Skip to NS char indicator
    while (rxBuffer[idx] != ',') idx++;
    idx++; // Skip over comma
    tempData.latitude *= (rxBuffer[idx] == 'N') ? 1 : -1;

    return true;
}

bool GPS::getLongitudeRMC(int &idx) {
    float lon = 0;
    for (int i = 0; i < 3; i++, idx++) {
        lon *= 10;
        lon += ((float)(rxBuffer[idx] - '0'));
    }

    float lon_minutes = 0;
    while (rxBuffer[idx] != '.') {
        lon_minutes *= 10;
        lon_minutes += ((float)(rxBuffer[idx] - '0'));
        idx++;
    }

    idx++; // Skip decimal char

    // Including two digits of minutes
    uint32_t mult = 10;
    while (rxBuffer[idx] != ',' && mult <= DECIMAL_PRECISION) {
        lon_minutes += ((float)(rxBuffer[idx] - '0')) / mult;
        idx++;
        mult *= 10;
    }

    lon += lon_minutes / 60;

    tempData.longitude = lon;
    while (rxBuffer[idx] != ',') idx++;
    idx++;
    tempData.longitude *= (rxBuffer[idx] == 'E') ? 1 : -1;

    return true;
}

bool GPS::getSpeedRMC(int &idx) {
    float spd = 0;
    while (rxBuffer[idx] != '.') {
        spd *= 10;
        spd += rxBuffer[idx] - '0';
        idx++;
    }
    idx++; // Decimal char
    uint32_t mult = 10;
    while (rxBuffer[idx] != ',' && mult <= DECIMAL_PRECISION) {
        spd += ((float)(rxBuffer[idx] - '0')) / mult;
        idx++;
        mult *= 10;
    }

    tempData.groundSpeed = spd * 51.4444; // Convert from kt to cm/s

    return true;
}

bool GPS::getTrackAngleRMC(int &idx) {
    float cog = 0;
    // Check if cog was calculated
    if (rxBuffer[idx] != ',') {
        while (rxBuffer[idx] != '.') {
            cog *= 10;
            cog += rxBuffer[idx] - '0';
            idx++;
        }
        idx++; // Decimal char
        uint32_t mult = 10;
        while (rxBuffer[idx] != ',' && mult <= DECIMAL_PRECISION) {
            cog += ((float)(rxBuffer[idx] - '0')) / mult;
            idx++;
            mult *= 10;
        }
    }
    else {
        cog = INVALID_TRACK_ANGLE;
    }

    tempData.trackAngle = cog;

    return true;
}

bool GPS::getDateRMC(int &idx) {
    int day = (rxBuffer[idx] - '0') * 10 + rxBuffer[idx + 1] - '0';
    idx += 2;
    int month = (rxBuffer[idx] - '0') * 10 + rxBuffer[idx + 1] - '0';
    idx += 2;
    int year = (rxBuffer[idx] - '0') * 10 + rxBuffer[idx + 1] - '0';

    tempData.time.day = day;
    tempData.time.month= month;
    tempData.time.year = year;

    return true;
}

bool GPS::getNumSatellitesGGA(int &idx) {
    int numSats = 0;
    while (rxBuffer[idx] != ',') {
        numSats *= 10;
        numSats += rxBuffer[idx] - '0';
        idx++;
    }

    tempData.numSatellites = numSats;

    return true;
}

bool GPS::getAltitudeGGA(int &idx) {
    float altitude = 0;
    while (rxBuffer[idx] != '.') {
        altitude *= 10;
        altitude += rxBuffer[idx] - '0';
        idx++;
    }
    idx++; // Decimal char
    uint32_t mult = 10;
    while (rxBuffer[idx] != ',' && mult <= DECIMAL_PRECISION) {
        altitude += ((float)(rxBuffer[idx] - '0')) / mult;
        idx++;
        mult *= 10;
    }

    tempData.altitude = altitude;
    return true;
}


bool GPS::getVx(int &idx) {
    int32_t ecefVX = (int32_t)((uint32_t)rxBuffer[idx] |
                               ((uint32_t)rxBuffer[idx+1] << 8) |
                               ((uint32_t)rxBuffer[idx+2] << 16) |
                               ((uint32_t)rxBuffer[idx+3] << 24));

    tempData.vx = ecefVX / 100.0f;
    return true;
}

bool GPS::getVy(int &idx) {
    int32_t ecefVY = (int32_t)((uint32_t)rxBuffer[idx] |
                               ((uint32_t)rxBuffer[idx+1] << 8) |
                               ((uint32_t)rxBuffer[idx+2] << 16) |
                               ((uint32_t)rxBuffer[idx+3] << 24));

    tempData.vy = ecefVY / 100.0f;
    return true;
}

bool GPS::getVz(int &idx) {
    int32_t ecefVZ = (int32_t)((uint32_t)rxBuffer[idx] |
                               ((uint32_t)rxBuffer[idx+1] << 8) |
                               ((uint32_t)rxBuffer[idx+2] << 16) |
                               ((uint32_t)rxBuffer[idx+3] << 24));

    tempData.vz = ecefVZ / 100.0f;
    return true;
}
