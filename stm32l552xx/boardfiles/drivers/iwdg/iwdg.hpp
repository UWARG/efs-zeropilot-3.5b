#pragma once

#include "iwdg_ifaces.hpp"
#include "iwdg.h"

class IndependentWatchdog : public IIndependentWatchdog{
    private:
		IWDG_HandleTypeDef* watchdog_;
		uint32_t prescaler_;
		uint32_t window_;
		uint32_t reload_;

		bool counterCalculation(uint32_t timeout, uint32_t &prescaler, uint32_t &counter);
    public:
        IndependentWatchdog(uint32_t timeout);

        bool refreshWatchdog() override;
}


