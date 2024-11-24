#ifndef MOTOR_IFACE_HPP
#define MOTOR_IFACE_HPP

class MotorControl {
public:
    virtual ~PWMChannel() = default;

    // Set the motor speed
    virtual void set(uint8_t percent) = 0;

};

#endif // MOTOR_CONTROL_HPP
