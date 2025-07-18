#pragma once

#include "flightmode.hpp"
#include "attitude_manager.hpp"
#include "pid.hpp"

class PIDMapping : public Flightmode {
    public:
        PIDMapping();

        RCMotorControlMessage_t runControl(RCMotorControlMessage_t controlInput) override;

        // Setter *roll* for PID consts
        void setRollPIDConstants(float newKp, float newKi, float newKd, float newTau) {
            rollPID->setConstants(newKp, newKi, newKd, newTau);
        }

        // Setter for *pitch* PID consts
        void setPitchPIDConstants(float newKp, float newKi, float newKd, float newTau) {
            pitchPID->setConstants(newKp, newKi, newKd, newTau);
        }

        // Destructor
        ~PIDMapping() override;
    
    private:

        // Roll integral limits
        const float ROLL_INTEGRAL_MIN_LIM = 0.0f;
        const float ROLL_INTEGRAL_MAX_LIM = 0.0f;
        
        // Pitch integral limits
        const float PITCH_INTEGRAL_MIN_LIM = 0.0f;
        const float PITCH_INTEGRAL_MAX_LIM = 0.0f;

        // Output limits (for control effort)
        const float OUTPUT_MIN = 0.0f;
        const float OUTPUT_MAX = 0.0f;

        // Roll and Pitch PID class objects
        PID *rollPID;
        PID *pitchPID;
};