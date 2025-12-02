#pragma once

#include "stm32l5xx_hal_i2c.h" //why is this an error 
#include "powerModule_iface.hpp"
#include "powerModule_defines.hpp"
#include <cmath> 


class PowerModule : public IPM {
    public:
        bool readData(PMData_t *data) override;
        PowerModule(I2C_HandleTypeDef *hi2c);
        bool init();


    private: 
        PMData_t validData;
        PMData_t tempData;
        I2C_HandleTypeDef *hi2c;
        bool writeRegister(uint16_t MemAddress, uint8_t * pData, uint16_t Size);
        bool readRegister(uint16_t MemAddress, uint8_t * pData, uint16_t Size);

};