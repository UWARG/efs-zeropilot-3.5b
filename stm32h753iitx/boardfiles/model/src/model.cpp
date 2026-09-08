#include "drivers.hpp"
#include "managers.hpp"
#include "zp_params.hpp"
#include "zp_bit.hpp"

void initModel()
{
  // Params first: initDrivers reads the servo table while constructing the motor handles.
  // BIT itself is started inside initDrivers, once SystemUtils provides its clock.
  ZP_Error paramStatus = ZP_PARAM::init();

  initDrivers();

  (void)ZP_BIT::report(ZP_BIT_ID::PARAM_TABLE_INIT, paramStatus);

  initManagers();
}