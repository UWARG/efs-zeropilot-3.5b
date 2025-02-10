#pragma once
#include "flightmode.hpp"


template <typename T>
class DirectMapping : public Flightmode<T> {
   public:
    DirectMapping() = default;
    virtual IMessageQueue<T>& run(const T& input) override;
};
