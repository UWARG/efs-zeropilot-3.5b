#pragma once


class PID {
    public:
        // PID object constructor
        PID(float kp, float ki, float kd, 
            float tau, float outputMinLim, float outputMaxLim,
            float integralMinLim, float integralMaxLim, float t);

        // For the PID roll & pitch consts -> may choose these to be optimized real-time dep. on optimization alg. chosen
        void setConstants(float newKp, float newKi, float newKd, float newTau);
        
        // PID object's state var initialized (or reset)
        void pidInitState();
        
        // Computes PID for a measurement with its desired setpoint passed in
        float pidOutput(float setpoint, float measurement);


    private:
        // Gains
        float kp, ki, kd;      // PID constants
        float tau;             // Derivative low-pass filter constant
        float t;               // Sample time (set to AM_MAIN_DELAY)

        // Output and Integral Limits
        float outputMinLim, outputMaxLim;       // Output limits
        float integralMinLim, integralMaxLim;   // integral limits

        // State variables
        float pidDerivative, pidIntegral;
        float prevError, prevMeasurement;
};
