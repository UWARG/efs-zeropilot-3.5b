#include "direct_mapping.hpp"
#include "rc_motor_control.hpp"

RCMotorControlMessage_t run_control(RCMotorControlMessage_t control_inputs){

        control_inputs = {
        control_inputs.roll, 
        control_inputs.pitch, 
        control_inputs.yaw, 
        control_inputs.throttle 
    };  

    return control_inputs;
}