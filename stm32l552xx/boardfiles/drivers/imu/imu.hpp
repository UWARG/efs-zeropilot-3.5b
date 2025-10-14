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

class IMU : public IIMU {
private:
	SPI_HandleTypeDef* _spi;
	GPIO_TypeDef* _csPort;
	uint16_t _csPin;

	// Internal helper functions
	void writeRegister(uint8_t subAddress, uint8_t data);
	int readRegisters(uint8_t subAddress, uint8_t count, uint8_t* dest);
	int setBank(uint8_t bank);
	void setLowNoiseMode();
	void reset();
	void readAGT(uint8_t* dataBuffer);

	// Calibration
	void calibrateGyro();
	void calibrateAccel();

	// Configuration
	void setAccelFS(uint8_t fssel);
	void setGyroFS(uint8_t fssel);
	
	// Utility
	uint8_t whoAmI();

	// Filtering
	float lowPassFilter(float raw_value, int select);
	void configureNotchFilter();
	void setAntiAliasFilter(uint16_t bandwidth_hz, bool accel_enable, bool gyro_enable);

	// Internal variables
	float _gyroScale;
	float _accelScale;
	uint8_t _gyroFS;
	uint8_t _accelFS;
	float _gyrB[3]; // currently not used to correct readings
	float _accB[3]; // currently not used to correct readings
	float _filteredGyro[3];
	float _alpha;


public:
	IMU(SPI_HandleTypeDef* spiHandle, GPIO_TypeDef* csPort, uint16_t csPin);

	// Initialization
	int init() override;

	// Data reading
	void getAccelGyro(float& ax, float& ay, float& az, float& gx, float& gy, float& gz) override;
};

#endif
