#pragma once

template <typename T>
class IMessageQueue {
    protected: 
        IMessageQueue() = default;
    public: 
        virtual ~IMessageQueue() = default;
        // retrieves the top of the queue
        virtual T get() = 0;
        // pushes template message to the back of the queue
        virtual T push(T message) = 0;
        // pops the top of the queue
        virtual T pop() = 0;
};