#pragma once

#include "sbus_datatypes.hpp"

class ISBUSReceiver { 
    protected:
        ISBUSReceiver() = default; 

    public:
        virtual ~ISBUSReceiver() = default;

        // get RCControl data that is parsed from sbus
        virtual RCControl_t getRCData() = 0;
};