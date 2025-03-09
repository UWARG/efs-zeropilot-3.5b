#include "telemetry_manager.hpp"
#define SYSTEM_ID 1     // Suggested System and
#define COMPONENT_ID 1  // Component ID values by MAVLINK



TelemetryManager::TelemetryManager() = default;

TelemetryManager::~TelemetryManager() = default;

void TelemetryManager::heartBeatMSGTx() {
    MAV_MODE_FLAG base_mode = MAV_MODE_FLAG_MANUAL_INPUT_ENABLED; // Ideally these two fields should be managed by something
    MAV_STATE system_status = MAV_STATE_STANDBY;                  // else like system manager

    mavlink_message_t heartbeat_message = {0};

    mavlink_msg_heartbeat_pack(SYSTEM_ID, COMPONENT_ID, &heartbeat_message, MAV_TYPE_QUADROTOR, MAV_AUTOPILOT_INVALID,
                               base_mode, 0, system_status);

    addMavlinkMsgToTxBuffer(heartbeat_message, highPriorityTxBuffer_);
}

bool TelemetryManager::addMavlinkMsgToTxBuffer(mavlink_message_t &msg, TMCircularBuffer &txBuffer) {
    uint8_t messageBuffer[MAVLINK_MAX_PACKET_LEN] = {};
    const uint16_t messageBufferLen = mavlink_msg_to_send_buffer(messageBuffer, &msg);
    txBuffer.messagesInQueue++;
    return txBuffer.enqueue(messageBuffer, messageBufferLen);

}

