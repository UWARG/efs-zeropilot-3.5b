#include "direct_mapping.hpp"
#include "rc_motor_control.hpp"

AttitudeManagerInput run_control(RCMotorControlMessage_t control_inputs){
    AttitudeManagerInput am_control_inputs_placeholder;

        am_control_inputs_placeholder = {
        control_inputs.roll, 
        control_inputs.pitch, 
        control_inputs.yaw, 
        control_inputs.throttle 
    };  

    return am_control_inputs_placeholder;
}