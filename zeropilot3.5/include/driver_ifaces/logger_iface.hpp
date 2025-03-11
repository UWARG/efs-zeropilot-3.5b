#pragma once

class ILogger {
    protected:
        ILogger() = default;
    
    public:
        virtual ~ILogger() = default;

        virtual int log(char message[100]) = 0;
        virtual int log(char message[][100], int count) = 0;
};
