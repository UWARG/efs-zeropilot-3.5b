#pragma once
#include "motor_iface.hpp"

class SITL_Motor : public IMotorControl {
public:
    SITL_Motor() = default;

    ZP_ERROR_e set(uint32_t percent) override {
        currentPercent = percent;
        return ZP_ERROR_OK;
    }

    ZP_ERROR_e init() override {
        return ZP_ERROR_OK;
    }

    uint32_t get() {
        return currentPercent;
    }

    void setArm(bool arm) override { 
        armFlag = arm;
    }

private:
    uint32_t currentPercent = 0;
};
