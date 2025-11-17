cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME stm32l552xx)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here

    # ZeroPilot
    "../zeropilot3.5/src/attitude_manager/attitude_manager.cpp"
    "../zeropilot3.5/src/attitude_manager/direct_mapping.cpp"
    "../zeropilot3.5/src/system_manager/system_manager.cpp"
    "../zeropilot3.5/src/telemetry_manager/telemetry_manager.cpp"

    # Boardfiles
    "boardfiles/drivers/gps/gps.cpp"
    "boardfiles/drivers/iwdg/iwdg.cpp"
    "boardfiles/drivers/logger/logger.cpp"
    "boardfiles/drivers/motor/motor.cpp"
    "boardfiles/drivers/rc/rc.cpp"
    "boardfiles/drivers/rfd/rfd.cpp"
    "boardfiles/drivers/systemutils/systemutils.cpp"
    "boardfiles/model/src/drivers.cpp"
    "boardfiles/model/src/managers.cpp"
    "boardfiles/model/src/model.cpp"
    "boardfiles/rtos/museq/museq.cpp"
    "boardfiles/rtos/threads/src/am_threads.cpp"
    "boardfiles/rtos/threads/src/sm_threads.cpp"
    "boardfiles/rtos/threads/src/startup_threads.cpp"
    "boardfiles/rtos/threads/src/tm_threads.cpp"
    "boardfiles/rtos/threads/src/unified_threads.cpp"
    "boardfiles/utils/utils.c"
    "boardfiles/override.cpp"
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths

    # ZeroPilot
    "../zeropilot3.5/include/attitude_manager"
    "../zeropilot3.5/include/driver_ifaces"
    "../zeropilot3.5/include/system_manager"
    "../zeropilot3.5/include/telemetry_manager"
    "../zeropilot3.5/include/thread_msgs"

    # Boardfiles
    "boardfiles/drivers/gps"
    "boardfiles/drivers/iwdg"
    "boardfiles/drivers/logger"
    "boardfiles/drivers/motor"
    "boardfiles/drivers/queue"
    "boardfiles/drivers/rc"
    "boardfiles/drivers/rfd"
    "boardfiles/drivers/systemutils"
    "boardfiles/model/inc"
    "boardfiles/rtos/museq"
    "boardfiles/rtos/threads/inc"
    "boardfiles/utils"

    # MAVLink
    "../external/c_library_v2/all"
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
