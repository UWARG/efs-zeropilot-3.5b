#include "queue.hpp"

template <typename T>
class MessageQueue : public IMessageQueue<T>{
   private:
      osMessageQueueId_t *queue_id;
   public: 
      MessageQueue(osMessageQueueId_t *queue_id): queue_id{queue_id} {}

      // gets top element of queue
      T get() override {
        T message;
        osStatus_t status = osMessageQueueGet(*queue_id, &message, nullptr, osWaitForever);

        if (status == osOK) {
            return message;
        }
        throw std::runtime_error("Failed to get message from queue");
        
      }
      // pushes template message to the back of the queue
      void push(T message) override {
        T message;
        osStatus_t status = osMessageQueuePut(*queue_id, &message, 0, 0);

        if (status != osOK) throw std::runtime_error("Failed to push message to queue");
      }

      // counts the # of items in the queue
      int count() override {
        return osMessageQueueGetCount(*queue_id);
      }
      // counts remaining capacity of the queue
      int remainingCapacity() override {
        return osMessageQueueGetSpace(*queue_id);
      }
};