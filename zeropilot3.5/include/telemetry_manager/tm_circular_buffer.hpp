#pragma once
#define MAX_MAVLINK_MSG_SIZE 280 // This includes Authentication, could be smaller
#define MAVLINK_MSG_LIMIT 40     // Value based on previous system, look into changing?

#include <cstdint>
#include <mavlink2/common/mavlink.h>

class TMCircularBuffer{
private:
    mavlink_message_t buf_[MAVLINK_MSG_LIMIT];
    uint8_t writePtr_ = 0;
    uint8_t readPtr_ = 0;
    uint8_t messagesInQueue = 0;
public:
    TMCircularBuffer();
    ~TMCircularBuffer();

    /**
     * @brief Dequeue a Mavlink message from the queue
     *
     * @param message Mavlink message alias that will have the value of the message written to it
     * @return bool True if message was dequeued, false if queue is empty
     */
    bool dequeue(mavlink_message_t &message);

    /**
     * @brief Enqueue a Mavlink message into the queue
     *
     * @param message The message to be enqueued
     *
     * @return bool True if the message was enqueued successfully, false otherwise
     */
    bool enqueue(mavlink_message_t &message);

    /**
     * @brief Read a Mavlink message without dequeueing it
     *
     * @param message Mavlink message alias that will have the value of the message written to it
     *
     * @param index Index to be read, default value is zero
     * @return bool True if the message was read successfully, false otherwise
     */

    bool peek(mavlink_message_t &message, uint8_t index=0);

    bool isFull();
    bool isEmpty();
};

