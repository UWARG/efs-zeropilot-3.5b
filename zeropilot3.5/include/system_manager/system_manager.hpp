#pragma once

#include "queue_iface.hpp"
#include "sbus_iface.hpp"
#include "rc_motor_control.hpp"

class SystemManager {
    public:
        SystemManager() = delete;
        SystemManager(ISBUSReceiver *rcDriver, IMessageQueue<RCMotorControlMessage_t> *queueDriver, uint32_t invalidThreshold);

        void SMUpdate(); // This function is the main function of SM, it should be called in the main loop of the system.

    private:
        ISBUSReceiver *rcDriver_;
        IMessageQueue<RCMotorControlMessage_t> *queueDriver_;
        uint32_t invalidThreshold_;

        uint32_t invalidRCCount_ = 0;

        void sendRCDataToAttitudeManager(const RCControl_t &rcData);
        void sendDisarmedToAttitudeManager();
};