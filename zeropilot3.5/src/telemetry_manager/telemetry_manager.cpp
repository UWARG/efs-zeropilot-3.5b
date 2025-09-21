#include "telemetry_manager.hpp"
#define SYSTEM_ID 1             // Suggested System ID by Mavlink
#define COMPONENT_ID 1          // Suggested Component ID by MAVLINK


TelemetryManager::TelemetryManager(
    IRFD *rfdDriver,
    IMessageQueue<TMMessage_t> *tmQueueDriver,
    IMessageQueue<RCMotorControlMessage_t> *amQueueDriver,
    IMessageQueue<mavlink_message_t> *messageBuffer
) :
    rfdDriver(rfdDriver),
    tmQueueDriver(tmQueueDriver),
    amQueueDriver(amQueueDriver),
    messageBuffer(messageBuffer) {
}

TelemetryManager::~TelemetryManager() = default;


void TelemetryManager::processMsgQueue() {
    while (tmQueueDriver->count() > 0) {
        mavlink_message_t mavlinkMessage = {};
        TMMessage_t tmqMessage = {};
        tmQueueDriver->get(&tmqMessage);

        switch (tmqMessage.dataType) {
            case TMMessage_t::GPOS_DATA: {
                auto gposData = tmqMessage.tmMessageData.gposData;
                mavlink_msg_global_position_int_pack(SYSTEM_ID, COMPONENT_ID, &mavlinkMessage, tmqMessage.timeBootMs,
                	gposData.lat, gposData.lon, gposData.alt, gposData.relativeAlt, gposData.vx, gposData.vy, gposData.vz, gposData.hdg);
                break; }
            case TMMessage_t::RC_DATA: {
                auto rcData = tmqMessage.tmMessageData.rcData;
                mavlink_msg_rc_channels_pack(SYSTEM_ID, COMPONENT_ID, &mavlinkMessage, tmqMessage.timeBootMs, 6,
                	rcData.roll, rcData.pitch, rcData.yaw, rcData.throttle, rcData.arm, rcData.flapAngle,  // Channel arrangement from system manager
                    UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX,  UINT16_MAX,  UINT16_MAX, UINT16_MAX, UINT8_MAX);
                break; }
            case TMMessage_t::BM_DATA: {
                auto bmData = tmqMessage.tmMessageData.bmData;
                mavlink_msg_battery_status_pack(SYSTEM_ID, COMPONENT_ID, &mavlinkMessage, 255, MAV_BATTERY_FUNCTION_UNKNOWN, MAV_BATTERY_TYPE_LIPO,
                	bmData.temperature, bmData.voltages, bmData.currentBattery, bmData.currentConsumed, bmData.energyConsumed, bmData.batteryRemaining,
					bmData.timeRemaining, bmData.chargeState, {}, 0, 0); }
            default: {}
                //WHOOPS
        }
        messageBuffer->push(&mavlinkMessage);
    }
}

void TelemetryManager::heartBeatMsg() {
    MAV_MODE_FLAG baseMode = MAV_MODE_FLAG_MANUAL_INPUT_ENABLED; // Ideally these two fields should be managed by something
    MAV_STATE systemStatus = MAV_STATE_STANDBY;                  // else like system manager

    mavlink_message_t heartbeatMessage = {0};

    mavlink_msg_heartbeat_pack(SYSTEM_ID, COMPONENT_ID, &heartbeatMessage, MAV_TYPE_QUADROTOR, MAV_AUTOPILOT_INVALID,
                               baseMode, 0, systemStatus);
    messageBuffer->push(&heartbeatMessage);
}

void TelemetryManager::transmit() {
    uint8_t transmitBuffer[MAVLINK_MSG_MAX_SIZE];
    mavlink_message_t msgToTx{};
        while (messageBuffer->count() > 0) {
            messageBuffer->get(&msgToTx);
            const uint8_t MSG_LEN = mavlink_msg_to_send_buffer(transmitBuffer, &msgToTx);
            rfdDriver->transmit(transmitBuffer, MSG_LEN);
        }
}

void TelemetryManager::reconstructMsg() {


    uint8_t rxBuffer[RX_BUFFER_LEN];

    const uint16_t RECEIVED_BYTES = rfdDriver->receive(rxBuffer, sizeof(rxBuffer));

    //Use mavlink_parse_char to process one byte at a time
    for (uint16_t i = 0; i < RECEIVED_BYTES; ++i) {
        if (mavlink_parse_char(0, rxBuffer[i], &message, &status)) {
            handleRxMsg(message);
            message = {};
        }
    }
}

void TelemetryManager::handleRxMsg(const mavlink_message_t &msg) {
    switch (msg.msgid) {
        case MAVLINK_MSG_ID_PARAM_SET:{
            float valueToSet;
            char paramToSet[MAVLINK_MAX_IDENTIFIER_LEN] = {};
            uint8_t valueType;
            valueToSet = mavlink_msg_param_set_get_param_value(&msg);
            valueType = mavlink_msg_param_set_get_param_type(&msg);

            if(paramToSet[0] == 'A'){ //Would prefer to do this using an ENUM LUT but if this is the only param being set its whatever
                RCMotorControlMessage_t armDisarmMsg{};
                armDisarmMsg.arm = valueToSet;
                amQueueDriver->push(&armDisarmMsg);
            }
            mavlink_message_t response = {};
            mavlink_msg_param_value_pack(SYSTEM_ID, COMPONENT_ID, &response, paramToSet, valueToSet, valueType, 1, 0);
            messageBuffer->push(&response);
            break;}
        default:
            break;
    }
}
