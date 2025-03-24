
#include "tm_circular_buffer.hpp"

TMCircularBuffer::TMCircularBuffer() = default;

TMCircularBuffer::~TMCircularBuffer() {
    // Destructor
}

bool TMCircularBuffer::dequeue(mavlink_message_t &message) {
	bool success{false};
    if(!isEmpty()){
    	message =  buf_[readPtr_];
        success = true;
        readPtr_ = (readPtr_ + 1) % MAVLINK_MSG_LIMIT;
        messagesInQueue--;
    }
    return success;
}

bool TMCircularBuffer::enqueue(mavlink_message_t &message){
	bool success{false};
    if(isFull()){
    	buf_[writePtr_] = message;
        success = true;
        writePtr_ = (writePtr_ + 1) % MAVLINK_MSG_LIMIT;
        messagesInQueue++;
    }
    return success;
}

bool TMCircularBuffer::peek(mavlink_message_t &message, uint8_t index) {
    bool success = true;

    if (index > messagesInQueue) {
        success = false;
    } else {
        message = buf_[(readPtr_ + index) % messagesInQueue];
    }

    return success;
}

bool TMCircularBuffer::isFull() {

    return (messagesInQueue >= MAVLINK_MSG_LIMIT);
}

bool TMCircularBuffer::isEmpty(){
	return messagesInQueue == 0;
}


