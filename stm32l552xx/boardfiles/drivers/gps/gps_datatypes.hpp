#pragma once

#include <cstdint>

typedef struct {
	uint8_t year, month, day, hour, minute, second;
} gpsTime_t;


typedef struct {
	gpsTime_t time;
	float latitude, longitude; // Positive lat -> N else S; Positive lon -> E else W;
	float groundSpeed;
	uint8_t numSatellites;
	float trackAngle;
	bool valid;
} gpsData_t;
