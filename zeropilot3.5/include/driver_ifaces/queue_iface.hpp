#pragma once

template <typename T>
class MessageQueue {
    protected: 
        MessageQueue() = default;
    public: 
        virtual ~MessageQueue() = default;
        // retrieves the top of the queue
        virtual T get() = 0;
        // pushes template message to the back of the queue
        virtual T push(T message) = 0;
        // pops the top of the queue
        virtual T pop() = 0;
};