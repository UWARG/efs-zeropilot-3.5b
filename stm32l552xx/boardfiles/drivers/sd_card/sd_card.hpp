#pragma once

#include "sd_card_iface.hpp"

class SDCard : public ISDCard {
    private:
        FATFS FatFs;
        FIL fil;
        char file[100];

    public:
        SDCard() = default;

        int log(char* message) override;
        int log(char message[][100], int count) override;

        int init();

};
