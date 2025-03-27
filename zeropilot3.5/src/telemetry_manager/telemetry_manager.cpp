#include "telemetry_manager.hpp"
#define SYSTEM_ID 1             // Suggested System ID by Mavlink
#define COMPONENT_ID 1          // Suggested Component ID by MAVLINK


TelemetryManager::TelemetryManager(IRFD *rfdDriver, IMessageQueue<TMMessage_t> *tmQueueDriver, IMessageQueue<mavlink_message_t> *messageBuffer):
    tmQueueDriver_(tmQueueDriver), rfdDriver_(rfdDriver), messageBuffer_(messageBuffer) {

}

TelemetryManager::~TelemetryManager() = default;


void TelemetryManager::processMsgQueue() {
    while (tmQueueDriver_->count() > 0) {
        mavlink_message_t mavlink_message = {};
        TMMessage_t tmq_message = tmQueueDriver_->pop();

        switch (tmq_message.DataType) {
            case TMMessage_t::GPOS_DATA:
                auto GPOSData = tmq_message.tm_message_data.GPOSData_t;
                mavlink_msg_global_position_int_pack(SYSTEM_ID, COMPONENT_ID, &mavlink_message,tmq_message.time_boot_ms,
                    GPOSData.lat, GPOSData.lon, GPOSData.alt, GPOSData.relative_alt, GPOSData.vx, GPOSData.vy, GPOSData.vz, GPOSData.hdg);
                break;
            case TMMessage_t::AM_DATA:
                auto AMData = tmq_message.tm_message_data.AMData_t;
                mavlink_msg_attitude_pack(SYSTEM_ID, COMPONENT_ID, &mavlink_message, tmq_message.time_boot_ms,
                    AMData.roll, AMData.pitch, AMData.yaw, AMData.rollspeed, AMData.pitchspeed, AMData.yawspeed);
                break;
            case TMMessage_t::BM_DATA: //Basically a placeholder, Check in with BM team later
                auto BMData = tmq_message.tm_message_data.BMData_t;
                mavlink_msg_battery_status_pack(SYSTEM_ID, COMPONENT_ID, &mavlink_message, 255, MAV_BATTERY_FUNCTION_UNKNOWN, MAV_BATTERY_TYPE_LIPO,
                 BMData.temperature, BMData.voltages, BMData.current_battery, BMData.current_consumed, BMData.energy_consumed, BMData.battery_remaining,
                 BMData.time_remaining, BMData.charge_state, BMData.voltages_ext, 0, 0);
            case default:
                //WHOOPS
        }
        messageBuffer_->push(mavlink_message);
    }
}

void TelemetryManager::heartBeatMsg() {
    MAV_MODE_FLAG base_mode = MAV_MODE_FLAG_MANUAL_INPUT_ENABLED; // Ideally these two fields should be managed by something
    MAV_STATE system_status = MAV_STATE_STANDBY;                  // else like system manager

    mavlink_message_t heartbeat_message = {0};

    mavlink_msg_heartbeat_pack(SYSTEM_ID, COMPONENT_ID, &heartbeat_message, MAV_TYPE_QUADROTOR, MAV_AUTOPILOT_INVALID,
                               base_mode, 0, system_status);
    messageBuffer_->push(heartbeat_message);
}

void TelemetryManager::transmit() {
    uint8_t transmit_buffer[MAVLINK_MSG_MAX_SIZE];
    mavlink_message_t msg_to_tx{};
        while (messageBuffer_->count() > 0) {
            msg_to_tx = messageBuffer_->pop();
            const uint8_t msg_len = mavlink_msg_to_send_buffer(transmit_buffer, &msg_to_tx);
            rfdDriver_->transmit(transmit_buffer, msg_len);
        }
}


