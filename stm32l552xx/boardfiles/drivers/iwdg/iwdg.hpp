#pragma once

#include "iwdg_ifaces.hpp"
#include "iwdg.h"

class IndependentWatchdog : public IIndependentWatchdog{
    private:
		IWDG_HandleTypeDef* watchdog_;
    public:
        IndependentWatchdog();

        bool refreshWatchdog() override;
}


