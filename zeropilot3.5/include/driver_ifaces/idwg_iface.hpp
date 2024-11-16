#pragma once

#include <cstdint>

class IndependentWatchdog{
    protected:
        IndependentWatchdog() = default; //do we need this if we don't call it
    public:
        virtual ~IndependentWatchdog() = default;
        virtual refreshWatchdog() = 0;
}