#pragma once

#include "iwdg_ifaces.hpp"
#include "iwdg.h" //what for

class IndependentWatchdog : public IIndependentWatchdog{
    private:
		IWDG_HandleTypeDef* watchdog_;
		uint32_t prescaler_;
		uint32_t window_;
		uint32_t reload_;

		bool counterCalculation(uint32_t timeout, uint32_t &prescaler, uint32_t &counter);
		bool windowCalculation(uint32_t timeout, uint32_t prescaler, uint32_t &window);
    public:
        IndependentWatchdog(uint32_t timeout);
        IndependentWatchdog(uint32_t counter_timeout, uint32_t window_timeout);

        bool refreshWatchdog() override;
}


