#ifndef INC_LOGGER_HPP_
#define INC_LOGGER_HPP_

#pragma once

#include "logger_iface.hpp"
#include "app_fatfs.h"
#include "cmsis_os.h"
#include "config.h"
#include <string.h>
#include <stdio.h>

class Logger : ILogger {
    private:
        FATFS FatFs;
        FIL fil;
        char file[100];

    public:
        Logger() = default;

        /**
         * @brief logs a single message to the SD Card
         * @param message: data to be written
         * @retval DRESULT: Operation result
         */
        int log(char* message);

        /**
         * @brief logs multiple messages to the SD card
         * @param messages: data to be written
         * @retval DRESULT: Operation result
         */
        int log(char messages[][100], int count);

        /**
         * @brief mounts SD card and selects file to write to, call before starting kernel
         */
        int init();

};

#endif /* INC_LOGGER_HPP_ */
