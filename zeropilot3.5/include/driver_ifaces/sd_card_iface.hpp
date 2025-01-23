#pragma once

class ISDCard {
    protected:
        ISDCard() = default;
    
    public:
        virtual ~ISDCard() = default;

        //log data to SD card
        virtual int log(char* message) = 0;
        virtual int logMulti(char *message[], int num) = 0;
};