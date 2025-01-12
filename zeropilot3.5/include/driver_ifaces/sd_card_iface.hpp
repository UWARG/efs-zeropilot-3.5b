#pragma once

class ISDCard {
    protected:
        ISDCard() = default;
    
    public:
        virtual ~ISDCard() = default;

        //log data to SD card
        virtual void log() = 0;
};