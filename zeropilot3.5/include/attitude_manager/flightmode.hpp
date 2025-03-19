#pragma once

#include "attitude_manager.hpp"

class Flightmode {
    protected:
    Flightmode() = default;

   public:
    virtual ~Flightmode() = default;

    virtual RCMotorControlMessage_t run_control(RCMotorControlMessage_t control_input) = 0;
    //conversion happens here
    //run flightmode, get those computed inputs, put it into outputtomotor
};