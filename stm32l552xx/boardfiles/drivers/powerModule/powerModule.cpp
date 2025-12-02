#include "stm32l5xx_hal.h"

#include "powerModule.hpp"


PowerModule::PowerModule(I2C_HandleTypeDef* hi2c) : hi2c(hi2c) {}

bool PowerModule::init() {
    bool success = (HAL_I2C_IsDeviceReady(hi2c, INA228_ADDR << 1, 1, 100) == HAL_OK);
    uint8_t pData[2];
    

    //only return success if every write was successful
    uint16_t shuntCal = SHUNT_CAL_VALUE;
    pData[0] = (shuntCal >> 8) & 0xFF;  
    pData[1] = shuntCal & 0xFF;     
    success &= writeRegister(REG_SHUNT_CAL.address, pData, REG_SHUNT_CAL.byte_size);
    
    uint16_t adcConfig = ADC_CONFIG_VALUE;
    pData[0] = (adcConfig >> 8) & 0xFF;  
    pData[1] = adcConfig & 0xFF;         
    success &= writeRegister(REG_ADC_CONFIG.address, pData, REG_ADC_CONFIG.byte_size);
    
    uint16_t config = CONFIG_VALUE;
    pData[0] = (config >> 8) & 0xFF;  
    pData[1] = config & 0xFF;         
    success &= writeRegister(REG_CONFIG.address, pData, REG_CONFIG.byte_size);
    
    return success;
}
bool PowerModule::readData(PMData_t *data)
{
    uint8_t pData[5]; 
    

    //read the core data

    for(int i = 0; i < sizeof(READ_REGISTERS) / sizeof(READ_REGISTERS[0]); i++) {
        const RegInfo &reg = READ_REGISTERS[i];
        uint64_t raw = 0; 
        if(readRegister(reg.address, pData, reg.byte_size)) {
            for(int j = 0; j < reg.byte_size; j++) {
                 raw |= (uint64_t)(pData[j]) << (8 * (reg.byte_size - 1 - j));
            }
      
            //sign extension on the raw
            uint8_t sign_position = 8 * (reg.byte_size) - 1;
            //2. check if there is a 1 bit in the sign position
            if(raw & (1ULL << sign_position)) {
                raw |= ~((1ULL << (sign_position)) - 1ULL); 
                //this is probably wrong 
            } 
            


            switch (reg.address) {
                case REG_VBUS.address:  
                    data->bus_voltage = raw * VBUS_LSB;
                    break;
                case REG_VSHUNT.address:  
                    data->shunt_voltage = (int64_t)raw * VSHUNT_LSB;
                    break;
                case REG_DIETEMP.address:  
                    data->die_temp = (int16_t)raw * DIETEMP_LSB;
                    break;
                case REG_CURRENT.address:  
                    data->current = (int64_t)raw * CURRENT_LSB;
                    break;
                case REG_POWER.address:  
                    data->power = raw * POWER_LSB;
                    break;
                case REG_ENERGY.address:  
                    data->energy = raw * ENERGY_LSB;
                    break;
                case REG_CHARGE.address:  
                    data->charge = (int64_t)raw * CHARGE_LSB;
                    break;
                case REG_DIAG_ALRT.address:  
                    data->diag_alrt_flags = (uint16_t)raw;
                    data->conversion_ready    = (raw & (1 << 14)); 
                    data->energy_overflow     = (raw & (1 << 11)); 
                    data->charge_overflow     = (raw & (1 << 10)); 
                    data->math_error          = (raw & (1 << 9));   
                    //bit 8 is reserved
                    data->temp_overlimit      = (raw & (1 << 7));   
                    data->shunt_overvoltage   = (raw & (1 << 6));  
                    data->shunt_undervoltage  = (raw & (1 << 5));  
                    data->bus_overvoltage     = (raw & (1 << 4));  
                    data->bus_undervoltage    = (raw & (1 << 3));  
                    data->power_overlimit     = (raw & (1 << 2));  
                    break; 
                default:
                    break;
            }

        } else {
            return false;
        }
    }


    //i need to make sure all the reads complete before i return here 
    return true;

}

bool PowerModule::writeRegister(
                                uint16_t memAddress,
                                uint8_t * pData, 
                                uint16_t size) {
    
    return HAL_I2C_Mem_Write_DMA(hi2c, INA228_ADDR << 1, memAddress, I2C_MEMADD_SIZE_8BIT, pData, size) == HAL_OK;
    
}

bool PowerModule::readRegister(
                                uint16_t memAddress,
                                uint8_t * pData, 
                                uint16_t size) {
    
    return HAL_I2C_Mem_Read_DMA(hi2c, INA228_ADDR << 1, memAddress, I2C_MEMADD_SIZE_8BIT, pData, size) == HAL_OK;
    
}