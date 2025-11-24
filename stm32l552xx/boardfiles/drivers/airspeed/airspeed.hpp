#pragma once

#include "airspeed_iface.hpp"
#include "stm3215xx_hal.h"
#include <cmath>

enum class Status : uint8_t {
    Normal  = 0b00,
    Command = 0b01,
    Stale   = 0b10,
    Fault   = 0b11
};

struct airspeedData {
    double raw_press_ = 0;
    double raw_temp_ = 0;
    double processed_temp_ = 0;
    double processed_press_ = 0;
    double airspeed_ = 0; 
};

class airspeed : public airspeed_iface{
private:
     uint8_t DMA_RX_Buffer[4];
    uint8_t process_RX_Buffer[4];

    I2C_HandleTypeDef* hi2c;
    uint8_t devAddress;


    Status status_ = Status::Fault;
   
    airspeedData airspeedData_;

    // private getters - for debugging
    double get_raw_press() { return airspeedData_.raw_press_; }
    double get_raw_temp() { return airspeedData_.raw_temp_; }
    
public:
    airspeed(I2C_HandleTypeDef* i2c, uint8_t addr = 0x28) : airspeed_iface(), hi2c(i2c), devAddress(addr << 1) {}
    ~airspeed() = default;

    bool airspeedInit();
    bool getAirspeedData(double* data_out);

    //helper function to calculate airspeed 
    bool calculateAirspeed(double* data_out);

    bool getAirspeed(double *data) { *data = airspeedData_.airspeed_; return true; }
};

airspeed : public airspeed_iface::airspeed : public airspeed_iface(/* args */)
{
}

airspeed : public airspeed_iface::~airspeed : public airspeed_iface()
{
}
