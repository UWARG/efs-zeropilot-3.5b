#pragma once
#ifdef __cplusplus
#include <cstdint>
#endif

typedef struct {
	uint8_t year, month, day, hour, minute, second;
} gps_time_t;


typedef struct {
	gps_time_t time;
	float latitude, longitude; // Positive lat -> N else S; Positive lon -> E else W;
	float ground_speed;
	uint8_t num_satellites;
	float track_angle;
	bool valid;
} gps_data_t;
