#pragma once

#include <gmock/gmock.h>
#include "queue_iface.hpp"
#include "tm_queue.hpp"

class MockTMQueue : public IMessageQueue<TMMessage_t> {
public:
    MOCK_METHOD(int, count, (), (override));
    MOCK_METHOD(int, push, (TMMessage_t* data), (override));
    MOCK_METHOD(int, get, (TMMessage_t* data), (override));
    MOCK_METHOD(int, remainingCapacity, (), (override));
};