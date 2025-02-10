#include "direct_mapping.hpp"
#include "queue_iface.hpp"

template <typename T>
IMessageQueue<T>& DirectMapping<T>::run(const T& input) {
    return input; //directly outputs the given inputs to the motor
}