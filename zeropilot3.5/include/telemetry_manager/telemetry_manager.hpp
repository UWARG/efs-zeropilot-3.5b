#pragma once

#include <mavlink2/common/mavlink.h>
#include "tm_circular_buffer.hpp"
#include "queue_iface.hpp"
#include "tm_queue.hpp"
#include "rfd900_iface.hpp"
class TelemetryManager {
  private:
    TMCircularBuffer highPriorityTxBuffer_ = TMCircularBuffer(); // GPOS, Attitude and Heartbeat/Connection Messages
    TMCircularBuffer lowPriorityTxBuffer_ = TMCircularBuffer();  // Everything Else (to be defined)
    IMessageQueue<TMMessage_t> *tmQueueDriver_;                   // Driver that receives messages from other managers
    IRFD900 *rfdDriver_;                                         // Driver used to actually send mavlink messages

  public:
    TelemetryManager(IRFD900 *rfdDriver, IMessageQueue<TMMessage_t>  *tmQueueDriver);
    ~TelemetryManager();

    void processMsgQueue();
    void heartBeatMsg(); //temporary implementation
    void transmit();
};