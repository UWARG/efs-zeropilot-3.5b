#pragma once

#include "sd_card_iface.hpp"
#include "app_fatfs.h"
#include "cmsis_os.h"
#include <string.h>
#include <stdio.h>

class SDCard : public ISDCard {
    private:
        FATFS FatFs;
        FIL fil;
        char file[100];

    public:
        SDCard() = default;

        /**
         * @brief logs a single message to the SD Card
         * @param message: data to be written
         * @retval DRESULT: Operation result
         */
        int log(char* message) override;

        /**
         * @brief logs multiple messages to the SD card
         * @param messages: data to be written
         * @param count: number of messages
         * @retval DRESULT: Operation result
         */
        int log(char messages[][100], int count) override;

        /**
         * @brief mounts SD card and selects file to write to, call before starting kernel
         */
        int init();

};
