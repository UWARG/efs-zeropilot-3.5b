#pragma once

#include <cstdint>

class IIndependentWatchdog{
    protected:
        IIndependentWatchdog() = default; //do we need this if we don't call it
    public:
        virtual ~IIndependentWatchdog() = default;
        virtual refreshWatchdog() = 0;
}