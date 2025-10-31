#pragma once

#include "rc_motor_control.hpp"
#include "error.h"

class Flightmode {
    protected:
        Flightmode() = default;

    public:
        virtual ~Flightmode() = default;

        virtual ZP_ERROR_e runControl(RCMotorControlMessage_t *motorOutputs, RCMotorControlMessage_t controlInput) = 0;
};