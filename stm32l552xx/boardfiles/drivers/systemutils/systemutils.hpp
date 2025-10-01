#pragma once

#include "systemutils_iface.hpp"

class SystemUtils : ISystemUtils {
    public:
        SystemUtils() = default;
        void delay(uint32_t delay_ms) override;
        uint32_t getCurrentTimestamp() override;
};