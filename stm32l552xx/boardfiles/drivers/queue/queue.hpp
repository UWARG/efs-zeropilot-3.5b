#pragma once

#include "queue_iface.hpp"
#include "cmsis_os2.h"

template <typename T>
class MessageQueue {
   private:
      osMessageQueueId_t queue_id;
   public: 
      MessageQueue(osMessageQueueId_t* queue_id);

      /**
       * @brief Gets top element of queue
       * @param message:
       * @retval status code
       */
      int get(T message);

      /**
       * @brief pushes message to the back of the queue
       * @param message: data to be transmitted
       * @retval status code
       */
      int push(T message);

      /**
       * @brief returns the number of messages in the queue
       */
      int count();

      /**
       * @brief Returns remaining space left in the queue
       * @retval number of available slots for messages
       */
      int remainingCapacity();
};

#include "../Src/queue.tpp"
