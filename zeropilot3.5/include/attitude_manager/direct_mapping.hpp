#pragma once

#include "flightmode.hpp"

class DirectMapping final : public Flightmode {
    public:
        DirectMapping() = default;

        RCMotorControlMessage_t runControl(RCMotorControlMessage_t CONTROL_INPUTS) override;
};
