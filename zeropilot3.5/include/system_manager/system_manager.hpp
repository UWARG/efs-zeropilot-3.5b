#pragma once

#include "queue_iface.hpp"
#include "sbus_iface.hpp"
#include "rc_motor_control.hpp"

class SystemManager {
    public:
    SystemManager(SBusIface *rc_driver, QueueIface<RCMotorControlMessage_t> *queue_driver);
    ~SystemManager();

    void SMUpdate(); // This function is the main function of SM, it should be called in the main loop of the system.

    private:
        SBusIface *rc_driver_;
        QueueIface<RCMotorControlMessage_t> *queue_driver_;

        int16_t invalidRCCount_ = 0;

        void sendRCDataToAttitudeManager(const SBusIface::RCData_t &rcData);
        void sendDisarmedToAttitudeManager();
};
