#include <queue>
template <typename T>
class MessageQueue {
    queue<T> messages;
    public: 
        // retrieves the top of the queue
        T get();
        // pushes template message to the back of the queue
        T push(T message);
        // pops the top of the queue
        T pop();
};