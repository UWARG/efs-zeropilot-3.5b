#pragma once
#define MAVLINK_MSG_MAX_SIZE 280
#define MAVLINK_MAX_IDENTIFIER_LEN 17
#define RX_BUFFER_LEN 8192

#include "mavlink.h"
#include "queue_iface.hpp"
#include "tm_queue.hpp"
#include "rc_motor_control.hpp"
#include "rfd_iface.hpp"
class TelemetryManager {
  private:
    IRFD *rfdDriver;										// Driver used to actually send mavlink messages
    IMessageQueue<TMMessage_t> *tmQueueDriver;				// Driver that receives messages from other managers
    IMessageQueue<RCMotorControlMessage_t> *amQueueDriver;	// Driver that currently is only used to set arm/disarm
    IMessageQueue<mavlink_message_t> *messageBuffer{};		// GPOS, Attitude and Heartbeat/Connection Messages
    mavlink_status_t status;
    mavlink_message_t message;
    void handleRxMsg(const mavlink_message_t &msg);


  public:
    TelemetryManager(IRFD *rfdDriver, IMessageQueue<TMMessage_t>  *tmQueueDriver,  IMessageQueue<RCMotorControlMessage_t> *amQueueDriver,IMessageQueue<mavlink_message_t> *messageBuffer);
    ~TelemetryManager();
    void processMsgQueue();
    void heartBeatMsg();
    void gpsMsg();
    void transmit();
    void reconstructMsg();
};
