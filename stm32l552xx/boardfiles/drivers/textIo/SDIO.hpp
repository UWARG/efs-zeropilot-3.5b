#pragma once

#include "textio_iface.hpp"
#include "app_fatfs.h"

class SDIO : public ITextIO {
    public:
        SDIO(const *filename[100]) = default;
        int open() override;
        int close() override;
        char* read(char *buffer, size_t bufferSize) override;
        int write(const char *buffer) override;
        int seek(int offset) override;
        size_t tell() override;
        int eof() override;

    private:
        FATFS FatFs;
        FIL fil;
        char file[100];
};