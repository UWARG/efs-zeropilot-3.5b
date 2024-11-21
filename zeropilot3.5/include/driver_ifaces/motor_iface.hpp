#ifndef MOTOR_CONTROL_HPP
#define MOTOR_CONTROL_HPP


class MotorControl {
public:
    virtual ~MotorControl() = default;

    // Set the motor speed
    virtual void set(uint8_t percent) = 0;
    
protected:
    MotorControl() = default;
};

#endif // MOTOR_CONTROL_HPP
