#pragma once

#ifndef ZPSW3_AM_FLIGHTMODE_HPP
#define ZPSW3_AM_FLIGHTMODE_HPP

#include <queue_iface.hpp>

template <typename T>
class Flightmode {
    protected:
    Flightmode() = default;

   public:
    virtual ~Flightmode() = default;

    virtual IMessageQueue<T>& run(const T& input) = 0;
    // virtual void updatePid() = 0;
    // virtual void updatePidGains() = 0;
    // virtual void updateControlLimits(ControlLimits_t limits) = 0; //Woud we need those?

};


#endif  
