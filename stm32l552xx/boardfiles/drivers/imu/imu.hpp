// IMU.hpp

#ifndef IMU_HPP
#define IMU_HPP

#include "imu_iface.hpp"
#include "stm32l5xx_hal.h"
#include <cstdint>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "imu_datatypes.hpp"

class IMU : public IIMU {
private:
	SPI_HandleTypeDef* _spi;
	GPIO_TypeDef* _csPort;
	uint16_t _csPin;

	inline static constexpr int RX_BUFFER_SIZE = 15; // inline static constexpr so it doesn't polluate namespace
	uint8_t imu_tx_buffer[RX_BUFFER_SIZE]; // only first bit register addr to read sensor data, rest 0
	uint8_t imu_rx_buffer[RX_BUFFER_SIZE]; // first byte is dummy, next 14 bytes are data received

	uint8_t curr_register_bank = 0;
	uint8_t state = 0;
	volatile uint8_t spi_tx_rx_flag = 0;
	IMUData_t imu_data = {}; // zero-initialize all floats

	
	// Utility functions
	void writeRegister(uint8_t bank, uint8_t register_addr, uint8_t data); // blocking
	int readRegister(uint8_t bank, uint8_t register_addr, uint8_t* data); // blocking
	
	void csLow();
	void csHigh();
	int setBank(uint8_t bank);
	void reset();
	uint8_t whoAmI();
	void processData();

	// Configuration
	void setLowNoiseMode();

	// Filtering
	float lowPassFilter(float raw_value, int select);
	
	// Internal variables
	float _alpha;
	float _filteredGyro[3];

	// TODO: below code needs to be tested and verified

	/*
	// Calibration
	void calibrateGyro();
	void calibrateAccel();

	// Configuration
	void setAccelFS(uint8_t fssel);
	void setGyroFS(uint8_t fssel);

	// Filtering
	void configureNotchFilter();
	void setAntiAliasFilter(uint16_t bandwidth_hz, bool accel_enable, bool gyro_enable);

	// Internal variables
	float _gyroScale;
	float _accelScale;
	uint8_t _gyroFS;
	uint8_t _accelFS;
	float _gyrB[3]; // currently not used to correct readings
	float _accB[3]; // currently not used to correct readings
	*/

public:
	IMU(SPI_HandleTypeDef* spiHandle, GPIO_TypeDef* csPort, uint16_t csPin);

	// Initialization
	int init() override;

	// Data reading
	IMUData_t readRawData() override; // non-blocking

	// put this in void HAL_SPI_TxRxCpltCallback (SPI_HandleTypeDef * hspi)
	void txRxCallback();
};

#endif
