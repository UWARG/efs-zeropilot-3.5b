#include "airspeed.hpp"
/*
airspeedInit - gets the initial data, confirms that data is good
getData - runs only if airspeedInit gives HAL_OK
*/

bool airspeed::airspeedInit() {
    HAL_StatusTypeDef success = HAL_I2C_Master_Receive_DMA(hi2c, devAddress, DMA_RX_Buffer, sizeof(DMA_RX_Buffer));
    return success == HAL_OK;
}

bool airspeed::getAirspeedData(double* data_out) {

    if (airspeedInit()) {
        calculateAirspeed(data_out);
        return true;
    }

    return false; // Will send a proper error message later
}

bool airspeed::calculateAirspeed(double* data_out) {

    HAL_StatusTypeDef success = HAL_I2C_Master_Receive_DMA(hi2c, devAddress, DMA_RX_Buffer, sizeof(DMA_RX_Buffer));
    HAL_I2C_MasterRxCpltCallback(hi2c);
    __HAL_I2C_DISABLE_IT(hi2c, I2C_IT_RXI);

    status_ = static_cast<Status>((process_RX_Buffer[0] >> 6) & 0x03);
    airspeedData_.raw_press_ = (((process_RX_Buffer[0] & 0x3F) << 8) | (process_RX_Buffer[1]));
    airspeedData_.raw_temp_ = ((process_RX_Buffer[2] << 3) | (((process_RX_Buffer[3] & 0xE0) >> 5) & 0x03));

    if(status_ != Status::Normal) {
        return false; // something wrong with the data 
    }

    // -- Calculations -- //

    //process raw temperature and pressure data
    airspeedData_.processed_temp_ = (airspeedData_.raw_temp_ * 200)/2047 - 50; // calculate temperature in deg C

    //calculate pressure in psi
    //Assumptions: output type A, 30 psi pressure range, differential mode 
    airspeedData_.processed_press_ = (airspeedData_.raw_press_ - 1638.3)/13106.4 + 19; 
    //convert pressure to Pa
    airspeedData_.processed_press_ = airspeedData_.processed_press_ * 6894.76;

    double air_density = 101325.0 / (287.058 * (airspeedData_.processed_temp_ + 273.15)); //calculate air density in kg/m^3, assuming stanard air pressure of 101.325 kPa and specific gas constant for dry air R = 287.058 J/(kg·K)

    //calculate airspeed in m/s using Bernoulli's equation
    airspeedData_.airspeed_ = std::sqrt((2 * airspeedData_.processed_press_) / air_density);

    __HAL_I2C_ENABLE_IT(hi2c, I2C_IT_RXI);

    *data_out = airspeedData_.airspeed_;
    return true; // Will send a proper error message later
}