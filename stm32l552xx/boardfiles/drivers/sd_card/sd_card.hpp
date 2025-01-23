#pragma once

#include "sd_card_iface.hpp"

class SDCard : public ISDCard {
    public:
        SDCard();
        ~SDCard();

        int log(char* message) override;
        int logMulti(char *message[], int num) override;
};