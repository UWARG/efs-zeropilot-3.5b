#pragma once

#ifdef __cplusplus

#include "startup_threads.hpp"
#include "am_threads.hpp"
#include "sm_threads.hpp"
#include "tm_threads.hpp"

#endif

#ifdef __cplusplus
extern "C" {
#endif

    void initThreads();

#ifdef __cplusplus
}
#endif