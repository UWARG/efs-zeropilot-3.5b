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

	return success == HAL_OK;
}

GpsData_t GPS::readData() {
	__HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_TC);
	
	GpsData_t data = validData;
	data.isNew = false;
	
	__HAL_DMA_ENABLE_IT(huart->hdmarx, DMA_IT_TC);

	return data;
}

bool GPS::processGPSData() {
	__HAL_DMA_DISABLE(huart->hdmarx);
	bool success = parseRMC() && parseGGA();
	tempData.isNew = success;

	validData = tempData;
	__HAL_DMA_ENABLE(huart->hdmarx);

	return success;
}

bool GPS::parseRMC() {
	int idx = 0;
	while (!(rxBuffer[idx] == 'R' && rxBuffer[idx+1] == 'M' && rxBuffer[idx+2] == 'C')) {
		idx++;
		if (idx == MAX_NMEA_DATA_LENGTH) return 0;
	}

	// Skip to time
	idx += 4;

	// Check if data exists
	if (rxBuffer[idx] == ',') {
		return 0;
	}

	// Begin time
	uint8_t hour = (rxBuffer[idx]-'0')*10 + (rxBuffer[idx+1]-'0');
	idx += 2;
	uint8_t minute = (rxBuffer[idx]-'0')*10 + (rxBuffer[idx+1]-'0');
	idx += 2;
	uint8_t second = (rxBuffer[idx]-'0')*10 + (rxBuffer[idx+1]-'0');

	tempData.time.hour = hour;
	tempData.time.minute = minute;
	tempData.time.second= second;
	// End time

	// Skip to status
	while (rxBuffer[idx] != ',') idx++;

	// Begin status
	idx++;

	// Check if data valid
	if (rxBuffer[idx] == 'V') return 0;
	// End status

	// Skip to latitude
	idx += 2;

	// Begin latitude
	float lat = 0;
	for (int i = 0; i < 2; i++, idx++) {
		lat *= 10;
		lat += ((float)(rxBuffer[idx]-'0'));
	}

	float lat_minutes = 0;
	while (rxBuffer[idx] != '.') {
		lat_minutes *= 10;
		lat_minutes += ((float)(rxBuffer[idx]-'0'));
		idx++;
	}
	idx++; // Skip decimal char

	// Including two digits of minutes
	uint32_t mult = 10;
	while (rxBuffer[idx] != ',' && mult <= DECIMAL_PRECISION) {
		lat_minutes += ((float)(rxBuffer[idx]-'0'))/mult;
		idx++;
		mult *= 10;
	}

	lat += lat_minutes/60;

	tempData.latitude = lat;

	// Skip to NS char indicator
	while (rxBuffer[idx] != ',') idx++;
	idx++; // Skip over comma
	tempData.latitude *= (rxBuffer[idx] == 'N') ? 1 : -1;
	// End latitude

	// Begin longitude
	idx+=2;

	float lon = 0;
	for (int i = 0; i < 3; i++, idx++) {
		lon *= 10;
		lon += ((float)(rxBuffer[idx]-'0'));
	}

	float lon_minutes = 0;
	while (rxBuffer[idx] != '.') {
		lon_minutes *= 10;
		lon_minutes += ((float)(rxBuffer[idx]-'0'));
		idx++;
	}
	idx++; // Skip decimal char

	// Including two digits of minutes
	mult = 10;
	while (rxBuffer[idx] != ',' && mult <= DECIMAL_PRECISION) {
		lon_minutes += ((float)(rxBuffer[idx]-'0'))/mult;
		idx++;
		mult *= 10;
	}

	lon += lon_minutes / 60;

	tempData.longitude = lon;
	while (rxBuffer[idx] != ',') idx++;
	idx++;
	tempData.longitude *= (rxBuffer[idx] == 'E') ? 1 : -1;;

	idx += 2;
	// End longitude

	// Begin speed
	float spd = 0;
	while (rxBuffer[idx] != '.') {
		spd *= 10;
		spd += rxBuffer[idx]-'0';
		idx++;
	}
	idx++; // Decimal char
	mult = 10;
	while (rxBuffer[idx] != ',' && mult <= DECIMAL_PRECISION) {
		spd += ((float)(rxBuffer[idx]-'0'))/mult;
		idx++;
		mult *= 10;
	}

	tempData.groundSpeed = spd;
	// End speed

	// Begin Course over Ground
	while (rxBuffer[idx] != ',') idx++;
	idx++;
	float cog = 0;
	// Check if cog was calculated
	if (rxBuffer[idx] != ',') {
		while (rxBuffer[idx] != '.') {
			cog *= 10;
			cog += rxBuffer[idx]-'0';
			idx++;
		}
		idx++; // Decimal char
		mult = 10;
		while (rxBuffer[idx] != ',' && mult <= DECIMAL_PRECISION) {
			cog += ((float)(rxBuffer[idx]-'0'))/mult;
			idx++;
			mult *= 10;
		}
	}
	else {
		cog = INVALID_TRACK_ANGLE;
	}

	tempData.trackAngle = cog;
	// End Course over Ground


	// Start date
	while (rxBuffer[idx] != ',') idx++;
	while (rxBuffer[idx] == ',') idx++;

	int day = (rxBuffer[idx] - '0') * 10 + rxBuffer[idx + 1] - '0';
	idx += 2;
	int month = (rxBuffer[idx] - '0') * 10 + rxBuffer[idx + 1] - '0';
	idx += 2;
	int year = (rxBuffer[idx] - '0') * 10 + rxBuffer[idx + 1] - '0';

	tempData.time.day = day;
	tempData.time.month= month;
	tempData.time.year = year;
	// End date

	return 1;
}


bool GPS::parseGGA() {
	int idx = 0;
	while (!(rxBuffer[idx] == 'G' && rxBuffer[idx + 1] == 'G' && rxBuffer[idx + 2] == 'A')) {
		idx++;
		if (idx == MAX_NMEA_DATA_LENGTH) return 0;
	}
	idx+=4; // Skip to data

	// Check if data exists
	if (rxBuffer[idx] == ',') {
		return 0;
	}

	// Skip 7 sections of data
	for (int i = 0; i < 6; i++, idx++) {
		while (rxBuffer[idx] != ',') idx++;
	}

	// Begin numSatellites
	int numSats = 0;
	while (rxBuffer[idx] != ',') {
		numSats *= 10;
		numSats += rxBuffer[idx] - '0';
		idx++;
	}

	tempData.numSatellites = numSats;
	// End numSatellites

	return 1;
}
