#pragma once

#include "queue_iface.hpp"
#include "cmsis_os2.h"

template <typename T>
class MessageQueue : public IMessageQueue<T>{
   private:
      osMessageQueueId_t *queue_id;
   public: 
      MessageQueue(osMessageQueueId_t *queue_id);

      // gets top element of queue
      T get() override;
      // pushes template message to the back of the queue
      T push(T message) override;
      // pops the top of the queue
      T pop() override;
      // counts the # of items in the queue
      int count() override;
      // counts remaining capacity of the queue
      int remainingCapacity() override;
};