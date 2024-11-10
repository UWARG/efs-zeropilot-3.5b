#ifndef ZPSW3_SM_HPP
#define ZPSW3_SM_HPP

#include "queue_iface.hpp"
#include "sbus_iface.hpp"
#include "attitude_manager.hpp"

namespace SM {

class SystemManager {
    public:
    SystemManager(SBusIface *rc_driver, QueueIface<AM::AttitudeManager::RCMotorControlMessage_t> *queue_driver);
    ~SystemManager();

    void SMUpdate(); // This function is the main function of SM, it should be called in the main loop of the system.

    private:
        SBusIface *rc_driver_;
        QueueIface<AM::AttitudeManager::RCMotorControlMessage_t> *queue_driver_;

        SBusIface::RCData_t lastRCData_;
        int16_t unchangedCount_ = 0;

        bool isRCDataChanged(const SBusIface::RCData_t &rcData) const;
        void sendRCDataToAttitudeManager(const SBusIface::RCData_t &rcData);
        void sendDisarmedToAttitudeManager();
};

} // namespace SM

#endif // ZPSW3_SM_HPP