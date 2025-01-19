#include "iwdg.hpp"
#include "stm32l552xx/Core/Inc/stm32l5xx_hal_conf.h"

#define MAX_PR 			6
#define RELOAD_LENGTH 	12
#define LSI_SPEED 		LSI_VALUE
#define PR_OFFSET		2
#define MAX_TIMEOUT_MS		(((1 << RELOAD_LENGTH)*4*(1 << MAX_PR)*1000) / LSI_SPEED) // should be about 32768

IndependentWatchdog::IndependentWatchdog(uint32_t timeout){
	if (timeout >= MAX_TIMEOUT_MS) {
		return;
	}

	if (counterCalculation(timeout, this->prescaler_, this->reload_) == false){
		// Error_Handler();
		return;
	}

	this->watchdog_ = &hiwdg;

	hiwdg.Instance = IWDG;
	hiwdg.Init.Prescaler = this->prescaler_;
	hiwdg.Init.Reload = this->reload_;
	if (HAL_IWDG_Init(this->watchdog_) != HAL_OK)
	{
		Error_Handler();
	}
}


bool IndependentWatchdog::refreshWatchdog() {
	if (this->watchdog_ == nullptr) {
		return false;
	}
	return (HAL_IWDG_Refresh(this->watchdog_) == HAL_OK);
}

bool IndependentWatchdog::counterCalculation(uint32_t timeout, uint32_t &prescaler, uint32_t &counter){
	int prescalerPR = 0; // start prescaler = 4
	const uint32_t MAX_COUNTER = (1 << RELOAD_LENGTH);

	for (; prescalerPR <= MAX_PR; prescalerPR++){
		prescaler = prescalerPR;
		uint32_t prescalerValue = 1 << (prescalerPR + PR_OFFSET);
		counter = ((timeout * LSI_SPEED) / (prescalerValue * 1000)) - 1;
		if (counter <=  MAX_COUNTER) {
			return true;
		}
	}

	return false;
}