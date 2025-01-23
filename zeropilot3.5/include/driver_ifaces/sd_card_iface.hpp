#pragma once

class ISDCard {
    protected:
        ISDCard() = default;
    
    public:
        virtual ~ISDCard() = default;

        virtual int log(char* message) = 0;
        virtual int log(char message[][100], int count) = 0;
};
