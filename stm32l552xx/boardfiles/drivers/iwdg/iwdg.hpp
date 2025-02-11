#pragma once

#include "iwdg_iface.hpp"
#include "main.h"

class IndependentWatchdog : public IIndependentWatchdog{
    private:
		    IWDG_HandleTypeDef* watchdog_;
    public:
        IndependentWatchdog(IWDG_HandleTypeDef *hiwdg);

        /**
         * @brief reset watchdog timer
         */
        bool refreshWatchdog() override;
};


