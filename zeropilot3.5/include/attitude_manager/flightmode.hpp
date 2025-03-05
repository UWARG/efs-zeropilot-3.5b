#pragma once

#ifndef ZPSW3_AM_FLIGHTMODE_HPP
#define ZPSW3_AM_FLIGHTMODE_HPP

#include "attitude_manager.hpp"

class Flightmode {
    protected:
    Flightmode() = default;

   public:
    virtual ~Flightmode() = default;

    virtual AttitudeManagerInput run_control(RCMotorControlMessage_t control_input);
    //conversion happens here
    //run flightmode, get those computed inputs, put it into outputtomotor
};

#endif  
