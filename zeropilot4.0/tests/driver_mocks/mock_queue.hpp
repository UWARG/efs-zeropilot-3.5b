#pragma once

#include <gmock/gmock.h>
#include "queue_iface.hpp"

template <typename T>
class MockMessageQueue : public IMessageQueue<T> {
    public:
        MOCK_METHOD(ZP_ERROR_e, get, (T *message), (override));
        MOCK_METHOD(ZP_ERROR_e, push, (T *message), (override));
        MOCK_METHOD(ZP_ERROR_e, count, (int &count_value), (override));
        MOCK_METHOD(ZP_ERROR_e, remainingCapacity, (int &capacity), (override));
};
