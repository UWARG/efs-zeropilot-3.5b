#include "unified_threads.hpp"

#ifdef __cplusplus
extern "C" {
#endif

  void initThreads()
  {
    startUpInitThreads();
    amInitThreads();
    smInitThreads();
    tmInitThreads();
  }

#ifdef __cplusplus
}
#endif
