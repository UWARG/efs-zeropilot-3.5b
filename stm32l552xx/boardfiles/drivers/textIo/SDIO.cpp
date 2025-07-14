#include "SDIO.hpp"

SDIO::SDIO(const char *filename) {
    // Initialize the file name with the provided filename
    strncpy(file, filename, sizeof(file) - 1);
    file[sizeof(file) - 1] = '\0'; // Ensure null termination
}

int SDIO::open() {
    if (f_open(&fil, file, FA_READ | FA_WRITE) != 0) {
        return 1; // Error opening file
    }
    return 0; // Success
}

int SDIO::close() {
    if (f_close(&fil) != 0) {
        return 1; // Error closing file
    }
    return 0; // Success
}

char* SDIO::read(char *buffer, size_t bufferSize) {
    return f_gets(buffer, bufferSize, &fil); // Reads a line from the file into the buffer
}

int SDIO::write(const char *buffer) {
    return f_puts(buffer, &fil); // Writes a string to the file
}

int SDIO::seek(int offset) {
    if (f_lseek(&fil, offset) != 0) {
        return 1; // Error seeking in file
    }
    return 0; // Success
}

size_t SDIO::tell() {
    return f_tell(&fil); // Returns the current position in the file
}

int SDIO::eof() {
    return f_eof(&fil); // Returns whether the end of file has been reached
}