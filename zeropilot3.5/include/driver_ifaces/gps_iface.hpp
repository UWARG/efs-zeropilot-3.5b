#pragma once

#include "gps_datatypes.hpp"

#ifdef __cplusplus
#include <cstdint>
#endif


class IGPS {
protected:
	IGPS() = default;

public:
	virtual ~IGPS() = default;

	virtual gpsData_t readData() = 0;
};
