#pragma once

#ifndef ZPSW3_AM_FLIGHTMODE_HPP
#define ZPSW3_AM_FLIGHTMODE_HPP

#include "queue_iface.hpp"


//Using templates for flexibility in choosing data types later.
template <typename T>
class Flightmode {
    protected:
    Flightmode() = default;

   public:
    virtual ~Flightmode() = default;

    virtual IMessageQueue<T>& run(const T& input) = 0;

    //Are we redesigning those functions? I recall the implementation is quite complex s.t you want to redesign it simpler.
    virtual void updatePid() = 0;
    virtual void updatePidGains() = 0;
    virtual void updateControlLimits(ControlLimits_t limits) = 0; 

};


#endif  
