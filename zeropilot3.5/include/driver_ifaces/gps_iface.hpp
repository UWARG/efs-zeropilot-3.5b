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

	virtual gps_data_t readData() = 0;
};
