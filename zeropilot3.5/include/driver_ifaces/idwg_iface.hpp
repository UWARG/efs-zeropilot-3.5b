#pragma once

class IIndependentWatchdog{
    protected:
        IIndependentWatchdog() = default;
    public:
        virtual ~IIndependentWatchdog() = default;
        virtual bool refreshWatchdog() = 0;
}
