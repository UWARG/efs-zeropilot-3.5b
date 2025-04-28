#pragma once
#define MAVLINK_MSG_MAX_SIZE 280
#include <mavlink2/common/mavlink.h>
#include "queue_iface.hpp"
#include "tm_queue.hpp"
#include "rfd_iface.hpp"
class TelemetryManager {
  private:
    IMessageQueue<mavlink_message_t> *messageBuffer_{};            // GPOS, Attitude and Heartbeat/Connection Messages
    IMessageQueue<TMMessage_t> *tmQueueDriver_;                  // Driver that receives messages from other managers
    IRFD *rfdDriver_;                                         // Driver used to actually send mavlink messages

  public:
    TelemetryManager(IRFD *rfdDriver, IMessageQueue<TMMessage_t>  *tmQueueDriver, IMessageQueue<mavlink_message_t> *messageBuffer);
    ~TelemetryManager();

    void processMsgQueue();
    void heartBeatMsg(); //temporary implementation
    void transmit();
    void reconstructMessage();
    void sendCmdFromMessage(const mavlink_message_t &msg);
};