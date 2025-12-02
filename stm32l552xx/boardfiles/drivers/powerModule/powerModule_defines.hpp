#pragma once

#include <cstdint>

struct RegInfo {
    uint8_t address;
    uint8_t byte_size;
    bool is_signed;
};

//group tgt all the data on each register
//double check these values later
//
static constexpr RegInfo REG_CONFIG          = {0x00, 2, false}; // 16
static constexpr RegInfo REG_ADC_CONFIG      = {0x01, 2, false}; // 16
static constexpr RegInfo REG_SHUNT_CAL       = {0x02, 2, false}; // 16
static constexpr RegInfo REG_SHUNT_TEMPCO    = {0x03, 2, false}; // 16
static constexpr RegInfo REG_VSHUNT          = {0x04, 3, true};  // 24 and signed
static constexpr RegInfo REG_VBUS            = {0x05, 3, false}; // 24
static constexpr RegInfo REG_DIETEMP         = {0x06, 2, true};  // 16 signed
static constexpr RegInfo REG_CURRENT         = {0x07, 3, true};  // 24
static constexpr RegInfo REG_POWER           = {0x08, 3, false}; // 24
static constexpr RegInfo REG_ENERGY          = {0x09, 5, false}; // 40
static constexpr RegInfo REG_CHARGE          = {0x0A, 5, true};  // 40
static constexpr RegInfo REG_DIAG_ALRT       = {0x0B, 2, false}; // 16
static constexpr RegInfo REG_SOVL            = {0x0C, 2, false}; // 16
static constexpr RegInfo REG_SUVL            = {0x0D, 2, false}; // 16
static constexpr RegInfo REG_BOVL            = {0x0E, 2, false}; // 16
static constexpr RegInfo REG_BUVL            = {0x0F, 2, false}; // 16
static constexpr RegInfo REG_TEMP_LIMIT      = {0x10, 2, false}; // 16
static constexpr RegInfo REG_PWR_LIMIT       = {0x11, 2, false}; // 16
static constexpr RegInfo REG_MANUFACTURER_ID = {0x3E, 2, false}; // 16
static constexpr RegInfo REG_DEVICE_ID       = {0x3F, 2, false}; // 16

//i can always add more later but these ones are probably the most important
static constexpr RegInfo READ_REGISTERS[] = {
    REG_VBUS,       
    REG_VSHUNT,     
    REG_DIETEMP,    
    REG_CURRENT,    
    REG_POWER,      
    REG_ENERGY,     
    REG_CHARGE,     
    REG_DIAG_ALRT   
};



const uint8_t INA228_ADDR = 1001011; //SDA TO SCL
//could also be SCL TO SDA then INA228_ADDR = 1001110 

static constexpr float RSHUNT = 0.1; //we gotta figure this value out 
static constexpr float IMAX = 120.0f;  //safety margin of 20 amperes i believe


static constexpr float CURRENT_LSB = IMAX / (1 << 19);  
//what im doing below is super scuffed how do I do it properly 
static constexpr uint16_t SHUNT_CAL_VALUE = static_cast<uint16_t>(static_cast<uint32_t>(13107.2e6 * CURRENT_LSB * RSHUNT));  

static constexpr float VBUS_LSB = 195.3125e-6f;  // 195.3125 uV per bit
static constexpr float VSHUNT_LSB = 312.5e-9f;    // 312.5 nV per bit
static constexpr float POWER_LSB = 3.2f * CURRENT_LSB;  
static constexpr float ENERGY_LSB = 16 * 3.2 * CURRENT_LSB;  
static constexpr float CHARGE_LSB = CURRENT_LSB;  
static constexpr float DIETEMP_LSB = 7.8125e-3f;  // 7.8125 m°C per bit 

//config values
static constexpr uint16_t CONFIG_VALUE = 0b0001000000000110;  
static constexpr uint16_t ADC_CONFIG_VALUE = 0b0000000000000000; //i need to talk to determine this 

//do I set the limits or does battery manager 