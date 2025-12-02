#pragma once

#include <cstdint>

typedef struct {
    //core results
    float bus_voltage = 0.0f;
    float shunt_voltage = 0.0f;
    float die_temp = 0;
    float current = 0;
    float power = 0;

    //accumullated results 

    float energy = 0;//raw value is 40 bits so be careful with conversion here i think
    float charge = 0; //the raw value is 40
    //should this be float or double 


    //status and diagnostic
    bool conversion_ready = false;
    bool math_error = false;
    bool charge_overflow = false;
    bool energy_overflow = false;

    //faults
    //remember i need to set the limits for this at some point probably
    bool shunt_overvoltage = false;
    bool shunt_undervoltage = false;
    bool bus_overvoltage = false;
    bool bus_undervoltage = false;
    bool power_overlimit = false;
    bool temp_overlimit = false;


    uint16_t diag_alrt_flags = false; 
    


} PMData_t;