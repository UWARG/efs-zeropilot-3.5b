#pragma once

class ISDCard {
    protected:
        ISDCard() = default;
    
    public:
        virtual ~ISDCard() = default;

        //log data to SD card
        virtual int log() = 0;
};