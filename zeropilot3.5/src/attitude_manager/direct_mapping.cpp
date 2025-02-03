#include "direct_mapping.hpp"
#include "queue_iface.hpp"


template <typename T>
IMessageQueue<T> DirectMapping::run(const IMessageQueue<T>& input) {
    return input;
}