#pragma once

#ifdef __cplusplus

#include <cstdint>
#endif


class IGPS {
protected:
	IGPS() = default;

public:
	virtual ~IGPS() = default;

	virtual int read() = 0;
};
