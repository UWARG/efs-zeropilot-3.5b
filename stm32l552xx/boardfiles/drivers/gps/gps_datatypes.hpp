#pragma once

#include <cstdint>

#define INVALID_TRACK_ANGLE -1

typedef struct {
	uint8_t year, month, day, hour, minute, second;
} GpsTime_t;


typedef struct {
	GpsTime_t time;
	float latitude, longitude; // Positive lat -> N else S; Positive lon -> E else W;
	float groundSpeed;
	uint8_t numSatellites;
	float trackAngle;
	bool valid;
} GpsData_t;
