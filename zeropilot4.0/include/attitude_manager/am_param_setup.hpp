#pragma once

#include <cstdint>
#include "zp_error.h"
#include "param_setup.hpp"

class AttitudeManager;

class AMParamSetup : public IParamSetup {
   public:
    explicit AMParamSetup(AttitudeManager* am);
    ZP_Error loadAllParams() override;
    ZP_Error bindAllParamCallbacks() override;

   private:
    AttitudeManager* am;

    // Flightmode param callbacks
    #ifdef PLANE
    static ZP_ERROR_e updatePIDRollKp(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updatePIDRollKi(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updatePIDRollKd(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updatePIDRollTau(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updatePIDRollIMax(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updatePIDRollFF(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updatePIDPitchKp(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updatePIDPitchKi(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updatePIDPitchKd(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updatePIDPitchTau(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updatePIDPitchIMax(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updatePIDPitchFF(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updateKffRddrmix(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updateRollLimitDeg(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updatePitchLimMaxDeg(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updatePitchLimMinDeg(AttitudeManager* ctx, float val);
    #endif
    #ifdef QUADCOPTER
    static ZP_ERROR_e updateRatePIDRollKp(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updateRatePIDRollKi(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updateRatePIDRollKd(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updateRatePIDRollTau(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updateRatePIDRollIMax(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updateRatePIDPitchKp(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updateRatePIDPitchKi(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updateRatePIDPitchKd(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updateRatePIDPitchTau(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updateRatePIDPitchIMax(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updateRatePIDYawKp(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updateRatePIDYawKi(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updateRatePIDYawKd(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updateRatePIDYawTau(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updateRatePIDYawIMax(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updateRollPitchLimitRate(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updateYawLimitRate(AttitudeManager* ctx, float val);

    static ZP_ERROR_e updateAngPIDRollKp(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updateAngPIDRollKi(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updateAngPIDRollKd(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updateAngPIDRollTau(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updateAngPIDRollIMax(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updateAngPIDPitchKp(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updateAngPIDPitchKi(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updateAngPIDPitchKd(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updateAngPIDPitchTau(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updateAngPIDPitchIMax(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updateRollPitchLimitAng(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updateMotSpinMin(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updateMotSpinMax(AttitudeManager* ctx, float val);
    static ZP_ERROR_e updateMotSpinArm(AttitudeManager* ctx, float val);
    #endif

    // FFT Harmonic Notch Filter param callbacks
    static ZP_Error updateHarmonicNotchEnabled(AttitudeManager* ctx, float val);
    static ZP_Error updateHarmonicNotchWindowSize(AttitudeManager* ctx, float val);
    static ZP_Error updateHarmonicNotchMinFreqHz(AttitudeManager* ctx, float val);
    static ZP_Error updateHarmonicNotchBandwidthHz(AttitudeManager* ctx, float val);
    static ZP_Error updateHarmonicNotchAttenuationDB(AttitudeManager* ctx, float val);
    static ZP_Error updateHarmonicNotchHarmonicsMask(AttitudeManager* ctx, float val);

    // Servo param callback helpers
    static ZP_Error setServoTrim(AttitudeManager* ctx, uint8_t ch, float val);
    static ZP_Error setServoMin(AttitudeManager* ctx, uint8_t ch, float val);
    static ZP_Error setServoMax(AttitudeManager* ctx, uint8_t ch, float val);
    static ZP_Error setServoReversed(AttitudeManager* ctx, uint8_t ch, float val);
    static ZP_Error setServoFunction(AttitudeManager* ctx, uint8_t ch, float val);

    // Compile-time: each instantiation is a distinct function pointer
    template <uint8_t Ch> static ZP_Error cbServoTrim(AttitudeManager* ctx, float v)     { return setServoTrim(ctx, Ch, v); }
    template <uint8_t Ch> static ZP_Error cbServoMin(AttitudeManager* ctx, float v)      { return setServoMin(ctx, Ch, v); }
    template <uint8_t Ch> static ZP_Error cbServoMax(AttitudeManager* ctx, float v)      { return setServoMax(ctx, Ch, v); }
    template <uint8_t Ch> static ZP_Error cbServoReversed(AttitudeManager* ctx, float v) { return setServoReversed(ctx, Ch, v); }
    template <uint8_t Ch> static ZP_Error cbServoFunction(AttitudeManager* ctx, float v) { return setServoFunction(ctx, Ch, v); }
};
