#pragma once
#include "flightmode.hpp"

class DirectMapping : public Flightmode {
   private:
    AttitudeManagerInput run_control(RCMotorControlMessage_t control_input);
};
