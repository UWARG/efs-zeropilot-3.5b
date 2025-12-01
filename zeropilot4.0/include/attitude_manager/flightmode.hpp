#pragma once

#include "rc_motor_control.hpp"

class Flightmode {
    protected:
        Flightmode() = default;

    public:
        virtual ~Flightmode() = default;

        virtual RCMotorControlMessage_t runControl(RCMotorControlMessage_t controlInput) = 0;
};