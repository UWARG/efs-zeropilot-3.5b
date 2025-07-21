#include "fwba_mapping.hpp"

PIDMapping::PIDMapping() {
    rollPID = new PID(0.0f, 0.0f, 0.0f,
        0.0f, OUTPUT_MIN, OUTPUT_MAX,
        ROLL_INTEGRAL_MIN_LIM, ROLL_INTEGRAL_MAX_LIM, AM_MAIN_DELAY);
      
    pitchPID = new PID(0.0f, 0.0f, 0.0f,
        0.0f, OUTPUT_MIN, OUTPUT_MAX,
        PITCH_INTEGRAL_MIN_LIM, PITCH_INTEGRAL_MAX_LIM, AM_MAIN_DELAY);
    rollPID->pidInitState();
    pitchPID->pidInitState();
}

PIDMapping::~PIDMapping() {
    delete rollPID;
    delete pitchPID;
}

RCMotorControlMessage_t PIDMapping::runControl(RCMotorControlMessage_t controlInputs){
    float rollSetpoint = controlInputs.roll;
    float pitchSetpoint = controlInputs.pitch;

    // Get from sensor fusion -- make sure to update include files when we get this
    float rollMeasured = 50.0;
    float pitchMeasured = 50.0;

    // Currently, roll & pitch outputs receive absolute roll & pitch angles, not relative to current position.
    float rollOutput = rollPID->pidOutput(rollSetpoint, rollMeasured);
    float pitchOutput = pitchPID->pidOutput(pitchSetpoint, pitchMeasured);

    controlInputs.roll = rollOutput; // setting desired roll angle
    controlInputs.pitch = pitchOutput; // setting desired pitch angle

    return controlInputs;
}

