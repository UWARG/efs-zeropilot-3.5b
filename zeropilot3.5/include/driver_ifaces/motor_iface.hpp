#pragma once

#include <cstdint>

class IMotorControl {
    protected: 
        IMotorControl() = default;
        
    public:
        virtual ~IMotorControl() = default;

        // set pwm percentage of servo motors
        virtual void set(uint8_t percent) = 0;
};
