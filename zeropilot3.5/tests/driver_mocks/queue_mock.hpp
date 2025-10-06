#pragma once

#include <gmock/gmock.h>
#include "queue_fake.hpp"

template <typename T>
class MockMessageQueue : public IMessageQueue<T> {
    private:
        FakeMessageQueue<T> fakeQueue;
    public:
        MOCK_METHOD(int, get, (T *message), (override));
        MOCK_METHOD(int, push, (T *message), (override));
        MOCK_METHOD(int, count, (), (override));
        MOCK_METHOD(int, remainingCapacity, (), (override));

        void delegateToFake() {
            ON_CALL(*this, get).WillByDefault([this] (T *message) {
                return fakeQueue.get(message);
            });
            ON_CALL(*this, push).WillByDefault([this] (T *message) {
                return fakeQueue.push(message);
            });
            ON_CALL(*this, count).WillByDefault([this] () {
                return fakeQueue.count();
            });
            ON_CALL(*this, remainingCapacity).WillByDefault([this] () {
                return fakeQueue.remainingCapacity();
            });
        }
};
