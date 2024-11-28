#include "rcreceiver_datatypes.hpp"

class RCReceiver{ 
    
    protected:
        RCReceiver(); 

    public:
        virtual ~RCReceiver() = default;

        // get RCControl data that is parsed from sbus
        virtual RCControl getRCData();
};