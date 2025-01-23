#include "sd_card.hpp"

int SDCard::log(char* message) {
  f_open(&fil, "write.txt", FA_WRITE | FA_OPEN_APPEND);
  f_puts(message, &fil);
  f_close(&fil);
}

int SDCard::logMulti(char *message[], int num) {
  f_open(&fil, "write.txt", FA_WRITE | FA_OPEN_APPEND);
  for (int i = 0; i < num; i++) {
    f_puts(message[i], &fil);
  }
  f_close(&fil);
}