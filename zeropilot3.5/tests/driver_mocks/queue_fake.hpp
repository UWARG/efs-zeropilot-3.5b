#pragma once

#include <gmock/gmock.h>
#include "queue_iface.hpp"

template <typename T>
class FakeMessageQueue : public IMessageQueue<T> {
    private:
        int msgCount;
    public:
        FakeMessageQueue() {
            msgCount = 0;
        };

        int count() override {
            return msgCount;
        }

        int push(T *message) override {
            msgCount++;
            return 0;
        }

        int get(T *message) override {
            msgCount--;
            return 0;
        }

        int remainingCapacity() override {
            return 100 - msgCount;
        }
};