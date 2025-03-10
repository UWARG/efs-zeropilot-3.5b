#pragma once

#include <cstdint>

#define INVALID_TRACK_ANGLE -1

typedef struct {
	uint8_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
} GpsTime_t;


/*
	Positive lat -> N else S
	Positive lon -> E else W
	Invalid track angle is indicated by INVALID_TRACK_ANGLE = -1
*/
typedef struct {
	GpsTime_t time;
	float latitude;
	float longitude;
	float groundSpeed;
	uint8_t numSatellites;
	float trackAngle;
	bool isNew;
} GpsData_t;
