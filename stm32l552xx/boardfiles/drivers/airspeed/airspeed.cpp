#include "airspeed.hpp"

void airspeed::getData() {
    if (rxBusy) return;
    rxDone = false;
    rxBusy = true;
    HAL_I2C_Master_Receive_IT (hi2c, devAddress, RX_Buffer, sizeof(RX_Buffer));

}

void airspeed::calculateAirspeed() { // This
    if(!rxDone) return;

    status_ = static_cast<Status>((RX_Buffer[0] >> 6) & 0x03);
    raw_press_ = (((RX_Buffer[0] & 0x3F) << 8) | (RX_Buffer[1]));
    raw_temp_ = ((RX_Buffer[2] & 0x3F << 8) | ((RX_Buffer[3] >> 5) & 0x03));

    if(status_ != Status::Normal) {
        return;
    }

    // -- Calculations -- //

    getData();
}
