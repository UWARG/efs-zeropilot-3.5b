#pragma once

#include <cstdint>
#include "flightmode.hpp"
#include "pid.hpp"

class FBWAMapping : public Flightmode {
    public:
        FBWAMapping(float control_iter_period_s) noexcept;


        ZP_ERROR_e activateFlightMode() override;

        ZP_ERROR_e runControl(RCMotorControlMessage_t &controlOutput, const RCMotorControlMessage_t controlInput, const DroneState_t &droneState) override;
        
        // Setter *roll* for PID consts
        ZP_ERROR_e setRollPIDConstants(float newKp, float newKi, float newKd, float newTau, uint8_t newIMaxPct) noexcept;

        // Setter for *pitch* PID consts
        ZP_ERROR_e setPitchPIDConstants(float newKp, float newKi, float newKd, float newTau, uint8_t newIMaxPct) noexcept;

        // Setter for *roll* FF const
        void setRollFFConstant(float newRollFFConst) noexcept;

        // Setter for *pitch* FF const
        void setPitchFFConstant(float newPitchFFConst) noexcept;

        // Setter for *yaw* rudder mixing const
        ZP_ERROR_e setYawRudderMixingConstant(float newMixingConst) noexcept;

        // Setter for *rollLimitRad*
        ZP_ERROR_e setRollLimitDeg(float newRollLimitDeg) noexcept;

        // Setter for *pitchLimitMaxRad*
        ZP_ERROR_e setPitchLimitMaxDeg(float newPitchLimitMaxDeg) noexcept;

        // Setter for *pitchLimitMinRad*
        ZP_ERROR_e setPitchLimitMinDeg(float newPitchLimitMinDeg) noexcept;

        // Resetter for both roll and pitch PIDs (needed for unit testing)
        ZP_ERROR_e resetControlLoopState() noexcept;

        // Getter for PID objects
        ZP_ERROR_e getRollPID(PID* &rollpid) noexcept;
        ZP_ERROR_e getPitchPID(PID* &pitchPID) noexcept;

        // Destructor
        ~FBWAMapping() noexcept override = default;

    private:
        // Control loop iter period (s)
        float controlIterPeriod;

        // Roll and Pitch PID class objects
        PID rollPID;
        PID pitchPID;

        // Feedforward (FF) constants
        float rollFF;
        float pitchFF;
        float ffLpfAlpha;

        // Yaw rudder mixing constant
        float yawRudderMixingConst;

        // Values for roll/pitch limits
        float rollLimitRad;
        float pitchLimitMaxRad;
        float pitchLimitMinRad;

        // Internal state variables for feedforward logic
        float prevRollSetpoint;
        float prevPitchSetpoint;
        float prevFilteredRollRate;
        float prevFilteredPitchRate;

        // Output limits (for control effort)
        static constexpr float OUTPUT_MIN = -1.0f;
        static constexpr float OUTPUT_MAX = +1.0f;

        // PID output scale and shift to convert from [-1,1] normalized range to [0,100] motor range
        static constexpr float FBWA_PID_OUTPUT_SCALE = 50.0f;
        static constexpr float FBWA_PID_OUTPUT_SHIFT = 50.0f;

        // Assumed normalized range of RC Input to be [0, 100]
        static constexpr float MAX_RC_INPUT_VAL = 100.0f;

        // Cutoff frequency for FF LPF
        static constexpr float FF_LPF_CUTOFF_FREQ = 10.0f;
};
