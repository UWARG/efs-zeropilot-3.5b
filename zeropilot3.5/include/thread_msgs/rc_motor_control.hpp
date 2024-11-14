#ifndef ZP3_RC_MOTOR_CONTROL_HPP
#define ZP3_RC_MOTOR_CONTROL_HPP

struct RCMotorControlMessage_t {
    float roll;
    float pitch;
    float yaw;
    float throttle;
};

#endif // ZP3_RC_MOTOR_CONTROL_HPP