template <typename T>
MessageQueue<T>::MessageQueue(osMessageQueueId_t *id) : queue_id{id} {}

// counts the # of items in the queue
template <typename T>
int MessageQueue<T>::count() {
  return osMessageQueueGetCount(*queue_id);
}

// counts remaining capacity of the queue
template <typename T>
int MessageQueue<T>::remainingCapacity() {
  return osMessageQueueGetSpace(*queue_id);
}

// gets top element of queue
template <typename T>
int MessageQueue<T>::get(T* message) {
  if (count() > 0) {
    return osMessageQueueGet(*queue_id, message, 0, osWaitForever);
  }

  return osError;
}

// pushes template message to the back of the queue
template <typename T>
int MessageQueue<T>::push(T* message) {
  if (remainingCapacity() > 0) {
    return osMessageQueuePut(queue_id, message, 0, osWaitForever);
  }

  return osError;
}
