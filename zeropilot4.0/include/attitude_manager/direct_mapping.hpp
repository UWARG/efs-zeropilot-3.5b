#pragma once

#include "flightmode.hpp"

class DirectMapping : public Flightmode {
    public:
        DirectMapping() = default;

        RCMotorControlMessage_t runControl(RCMotorControlMessage_t controlInput) override;
};
