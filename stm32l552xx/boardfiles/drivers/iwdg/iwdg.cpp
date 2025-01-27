#include "iwdg.hpp"
#include "main.c" //MX_IDWG_INIT is initialized here, not declared in main.h

IndependentWatchdog::IndependentWatchdog(uint32_t timeout){
	this->watchdog_ = &hiwdg;
	MX_IWDG_Init();
}

bool IndependentWatchdog::refreshWatchdog() {
	if (this->watchdog_ == nullptr) {
		return false;
	}
	return (HAL_IWDG_Refresh(this->watchdog_) == HAL_OK);
}