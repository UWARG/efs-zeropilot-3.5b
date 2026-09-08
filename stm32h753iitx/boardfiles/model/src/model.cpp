#include "drivers.hpp"
#include "managers.hpp"
#include "zp_params.hpp"
#include "zp_bit.hpp"

void initModel()
{
  // SystemUtils first: BIT needs its millisecond clock before any driver reports into it.
  initSystemUtils();
  (void)ZP_BIT::init(systemUtilsHandle);

  // Params before drivers: initDrivers reads the servo table while constructing motor handles
  ZP_Error paramStatus = ZP_PARAM::init();

  initDrivers();

  (void)ZP_BIT::report(ZP_BIT_ID::PARAM_TABLE_INIT, paramStatus);

  initManagers();
}
