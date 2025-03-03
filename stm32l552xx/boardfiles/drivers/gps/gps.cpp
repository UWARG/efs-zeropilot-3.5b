#include "stm32l5xx_hal.h"

#include "gps.hpp"


GPS::GPS(UART_HandleTypeDef* huart) : huart(huart) {}

int GPS::init() {
	HAL_StatusTypeDef success = HAL_UARTEx_ReceiveToIdle_DMA(
			huart,
			rx_buffer,
			MAX_NMEA_DATA_LENGTH
	);

	__HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);

	return success == HAL_OK;
}

gps_data_t GPS::readData() {
	__HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_TC);
	gps_data_t data = gps_data;
	__HAL_DMA_ENABLE_IT(huart->hdmarx, DMA_IT_TC);

	return data;
}

int GPS::processGPSData() {
	__HAL_DMA_DISABLE(huart->hdmarx);
	bool success = parseRMC() && parseGGA();
	gps_data.valid = success;
	__HAL_DMA_ENABLE(huart->hdmarx);

	return success;
}

int GPS::parseRMC() {
	int idx = 0;
	while (!(rx_buffer[idx] == 'R' && rx_buffer[idx+1] == 'M' && rx_buffer[idx+2] == 'C')) {
		idx++;
		if (idx == MAX_NMEA_DATA_LENGTH) return 0;
	}

	// Skip to time
	idx+=4;

	// Check if data exists
	if (rx_buffer[idx] == ',') {
		return 0;
	}

	// Begin time
	uint8_t hour = (rx_buffer[idx]-'0')*10 + (rx_buffer[idx+1]-'0');
	idx += 2;
	uint8_t minute = (rx_buffer[idx]-'0')*10 + (rx_buffer[idx+1]-'0');
	idx += 2;
	uint8_t second = (rx_buffer[idx]-'0')*10 + (rx_buffer[idx+1]-'0');

	gps_data.time.hour = hour;
	gps_data.time.minute = minute;
	gps_data.time.second= second;
	// End time

	// Skip to status
	while (rx_buffer[idx] != ',') idx++;

	// Begin status
	idx++;

	// Check if data valid
	if (rx_buffer[idx] == 'V') return 0;
	// End status

	// Skip to latitude
	idx += 2;

	// Begin latitude
	float lat = 0;
	for (int i = 0; i < 2; i++, idx++) {
		lat *= 10;
		lat += ((float)(rx_buffer[idx]-'0'));
	}

	float lat_minutes = 0;
	while (rx_buffer[idx] != '.') {
		lat_minutes *= 10;
		lat_minutes += ((float)(rx_buffer[idx]-'0'));
		idx++;
	}
	idx++; // Skip decimal char

	// Including two digits of minutes
	uint32_t mult = 10;
	while (rx_buffer[idx] != ',' && mult <= DECIMAL_PRECISION) {
		lat_minutes += ((float)(rx_buffer[idx]-'0'))/mult;
		idx++;
		mult *= 10;
	}

	lat += lat_minutes/60;

	gps_data.latitude = lat;

	// Skip to NS char indicator
	while (rx_buffer[idx] != ',') idx++;
	idx++; // Skip over comma
	gps_data.latitude *= (rx_buffer[idx] == 'N') ? 1 : -1;
	// End latitude

	// Begin longitude
	idx+=2;

	float lon = 0;
	for (int i = 0; i < 3; i++, idx++) {
		lon *= 10;
		lon += ((float)(rx_buffer[idx]-'0'));
	}

	float lon_minutes = 0;
	while (rx_buffer[idx] != '.') {
		lon_minutes *= 10;
		lon_minutes += ((float)(rx_buffer[idx]-'0'));
		idx++;
	}
	idx++; // Skip decimal char

	// Including two digits of minutes
	mult = 10;
	while (rx_buffer[idx] != ',' && mult <= DECIMAL_PRECISION) {
		lon_minutes += ((float)(rx_buffer[idx]-'0'))/mult;
		idx++;
		mult *= 10;
	}

	lon += lon_minutes/60;

	gps_data.longitude = lon;
	while (rx_buffer[idx] != ',') idx++;
	idx++;
	gps_data.longitude *= (rx_buffer[idx] == 'E') ? 1 : -1;;

	idx += 2;
	// End longitude

	// Begin speed
	float spd = 0;
	while (rx_buffer[idx] != '.') {
		spd *= 10;
		spd += rx_buffer[idx]-'0';
		idx++;
	}
	idx++; // Decimal char
	mult = 10;
	while (rx_buffer[idx] != ',' && mult <= DECIMAL_PRECISION) {
		spd += ((float)(rx_buffer[idx]-'0'))/mult;
		idx++;
		mult *= 10;
	}

	gps_data.ground_speed = spd;
	// End speed

	// Begin Course over Ground
	while (rx_buffer[idx] != ',') idx++;
	idx++;
	float cog = 0;
	// Check if cog was calculated
	if (rx_buffer[idx] != ',') {
		while (rx_buffer[idx] != '.') {
			cog *= 10;
			cog += rx_buffer[idx]-'0';
			idx++;
		}
		idx++; // Decimal char
		mult = 10;
		while (rx_buffer[idx] != ',' && mult <= DECIMAL_PRECISION) {
			cog += ((float)(rx_buffer[idx]-'0'))/mult;
			idx++;
			mult *= 10;
		}
	}
	else {
		cog = -1;
	}

	gps_data.track_angle = cog;
	// End Course over Ground


	// Start date
	while (rx_buffer[idx] != ',') idx++;
	while (rx_buffer[idx] == ',') idx++;

	int day = (rx_buffer[idx]-'0')*10 + rx_buffer[idx+1]-'0';
	idx += 2;
	int month = (rx_buffer[idx]-'0')*10 +rx_buffer[idx+1]-'0';
	idx += 2;
	int year = (rx_buffer[idx]-'0')*10 + rx_buffer[idx+1]-'0';

	gps_data.time.day = day;
	gps_data.time.month= month;
	gps_data.time.year = year;
	// End date

	return 1;
}


int GPS::parseGGA() {
	int idx = 0;
	while (!(rx_buffer[idx] == 'G' && rx_buffer[idx+1] == 'G' && rx_buffer[idx+2] == 'A')) {
		idx++;
		if (idx == MAX_NMEA_DATA_LENGTH) return 0;
	}
	idx+=4; // Skip to data

	// Check if data exists
	if (rx_buffer[idx] == ',') {
		return 0;
	}

	// Skip 7 sections of data
	for (int i = 0; i < 6; i++, idx++) {
		while (rx_buffer[idx] != ',') idx++;
	}

	// Begin numSatellites
	int numSats = 0;
	while (rx_buffer[idx] != ',') {
		numSats *= 10;
		numSats += rx_buffer[idx]-'0';
		idx++;
	}

	gps_data.num_satellites = numSats;
	// End numSatellites

	return 1;
}
