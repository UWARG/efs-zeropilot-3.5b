#pragma once

#include <cstdint>

class IIMU {
protected:
	IIMU() = default;

public:
    virtual ~IIMU() = default;
	virtual int init();
	virtual void getAccelGyro(float& ax, float& ay, float& az, float& gx, float& gy, float& gz);
};
