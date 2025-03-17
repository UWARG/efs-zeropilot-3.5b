#pragma once
#include "flightmode.hpp"

class DirectMapping : public Flightmode {
   public:
    RCMotorControlMessage_t run_control(RCMotorControlMessage_t control_input) override;
};
