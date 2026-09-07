#pragma once

#include "attitude_manager.hpp"
#include "system_manager.hpp"
#include "telemetry_manager.hpp"
#include "zp_error.h"

extern AttitudeManager *amHandle;
extern SystemManager *smHandle;
extern TelemetryManager *tmHandle;

void initManagers();
