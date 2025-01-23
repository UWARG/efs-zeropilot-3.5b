#include "sd_card.hpp"
#include "app_fatfs.h"
#include <stdio.h>

int SDCard::init() {
    FRESULT res;

    res = f_mount(&FatFs, "", 1);
    if (res != FR_OK) {
        return res;
    }

    FRESULT exist = FR_OK;
    FILINFO fno;
    int count = 1;

    while (exist == FR_OK) {
        snprintf(file, 100, "log%d.txt", count);
        exist = f_stat(file, &fno);
        count++;
    }

    return res;
}

int SDCard::log(char* message) {
    FRESULT res;

    res = f_open(&fil, file, FA_WRITE | FA_OPEN_APPEND);
    res = f_puts(message, &fil);
    res = f_close(&fil);

    return res;
}

int SDCard::log(char message[][100], int count) {
    FRESULT res;

    res = f_open(&fil, file, FA_WRITE | FA_OPEN_APPEND);

    for (int i = 0; i < count; i++) {
      res = f_puts(message[i], &fil);
    }

    res = f_close(&fil);

    return res;
}
