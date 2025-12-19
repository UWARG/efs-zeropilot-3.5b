#pragma once

#include "powerModule_datatypes.hpp"

class IPM {
protected:
    IPM() = default;

public:
    virtual ~IPM() = default;

    virtual bool readData(PMData_t *data) = 0;
};