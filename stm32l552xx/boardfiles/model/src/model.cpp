#include "drivers.hpp"
#include "managers.hpp"

#ifdef __cplusplus
extern "C" {
#endif

  void initModel()
  {
    initDrivers();
    initManagers();
  }

#ifdef __cplusplus
}
#endif
