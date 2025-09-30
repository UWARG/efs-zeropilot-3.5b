#pragma once

#include "airspeed_iface.hpp"
#include "stm32l5xx_hal.h"

enum class Status : uint8_t {
    Normal  = 0b00,
    Command = 0b01,
    Stale   = 0b10,
    Fault   = 0b11
};

class airspeed : public airspeed_iface
{
private:
    uint8_t RX_Buffer[4];
    I2C_HandleTypeDef* hi2c;
    uint8_t devAddress;

    volatile bool rxBusy = false;
    volatile bool rxDone = false;

    Status status_ = Status::Fault;
    uint16_t raw_press_ = 0;
    uint16_t raw_temp_ = 0;

public:
    airspeed(I2C_HandleTypeDef* i2c, uint8_t addr = 0x28) : hi2c(i2c), devAddress(addr << 1) {}
    ~airspeed() = default;

    void getData() override;
    void calculateAirspeed() override;

    // getters
    bool ready() { return rxDone; }
    Status status() { return status_; }
    uint16_t get_raw_press() { return raw_press_; }
    uint16_t get__raw_temp() { return raw_temp_; }
    
};

