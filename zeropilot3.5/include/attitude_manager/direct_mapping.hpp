#pragma once
#include "flightmode.hpp"

template <typename T>
class DirectMapping : public Flightmode {
   public:
    Manual() = default;

    virtual IMessageQueue<T>& run(const T& input) = 0;
    // void updatePid() override;
    // void updatePidGains() override;
    // void updateControlLimits(ControlLimits_t limits) override;
};
