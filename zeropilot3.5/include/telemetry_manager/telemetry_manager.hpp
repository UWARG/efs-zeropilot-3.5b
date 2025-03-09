#pragma once

#include <mavlink2/common/mavlink.h>
#include "tm_circular_buffer.hpp"
class TelemetryManager {
  private:
    TMCircularBuffer highPriorityTxBuffer_; // GPOS, Attitude and Heartbeat/Connection Messages
    TMCircularBuffer lowPriorityTxBuffer_;  // Everything Else

    bool addMavlinkMsgToTxBuffer(mavlink_message_t &msg, TMCircularBuffer &txBuffer);
  public:
    TelemetryManager();
    ~TelemetryManager();


    void heartBeatMSGTx(); //temporary implementation
};