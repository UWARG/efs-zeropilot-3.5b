#pragma once

#include <cstdint>

typedef struct {
    float xacc;
    float yacc;
    float zacc;
    float xgyro;
    float ygyro;
    float zgyro;
} IMUData_t;
