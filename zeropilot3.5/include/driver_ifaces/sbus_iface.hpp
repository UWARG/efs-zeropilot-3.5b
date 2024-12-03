#include "sbus_datatypes.hpp"

class IRCReceiver{ 
    
    protected:
        IRCReceiver() = default; 

    public:
        virtual ~IRCReceiver() = default;

        // get RCControl data that is parsed from sbus
        virtual RCControl_t getRCData() = 0;
};