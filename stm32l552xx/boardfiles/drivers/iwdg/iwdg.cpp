#include "iwdg.hpp"

IndependentWatchdog::IndependentWatchdog(uint32_t timeout){
	this->watchdog_ = &hiwdg;
	if (HAL_IWDG_Init(this->watchdog_) != HAL_OK){
		Error_Handler();
	}
}

bool IndependentWatchdog::refreshWatchdog() {
	if (this->watchdog_ == nullptr) {
		return false;
	}
	return (HAL_IWDG_Refresh(this->watchdog_) == HAL_OK);
}