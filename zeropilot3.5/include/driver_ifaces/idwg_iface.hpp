#pragma once

#include <cstdint>

class IIndependentWatchdog{
    protected:
        IIndependentWatchdog() = default;
    public:
        virtual ~IIndependentWatchdog() = default;
        virtual refreshWatchdog() = 0;
}
