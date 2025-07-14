#pragma once

#include "config_keys.hpp"

class IConfig {
    protected:
        IConfig() = default;
    
    public:
        virtual ~IConfig() = default;

        virtual int readParam(ConfigKey key, float &val) = 0;
        virtual int writeParam(ConfigKey key, float newValue) = 0;
};

