//=============================================================================================
// MahonyAHRS.h
//=============================================================================================
//
// Madgwick's implementation of Mayhony's AHRS algorithm.
// See: http://www.x-io.co.uk/open-source-imu-and-ahrs-algorithms/
//
// Date			Author			Notes
// 29/09/2011	SOH Madgwick    Initial release
// 02/10/2011	SOH Madgwick	Optimised for reduced CPU load
//
//=============================================================================================
#ifndef MahonyAHRS_h
#define MahonyAHRS_h
#include <math.h>
#include "imu_datatypes.hpp"

//--------------------------------------------------------------------------------------------
// Variable declaration

class Mahony {
private:
	float twoKp;		// 2 * proportional gain (Kp)
	float twoKi;		// 2 * integral gain (Ki)
	float q0, q1, q2, q3;	// quaternion of sensor frame relative to auxiliary frame
	float integralFBx, integralFBy, integralFBz;  // integral error terms scaled by Ki
	float invSampleFreq;
	float roll, pitch, yaw;
	static float invSqrt(float x);

//-------------------------------------------------------------------------------------------
// Function declarations

public:
	Mahony();
	void begin(float sampleFrequency) { invSampleFreq = 1.0f / sampleFrequency; }
	void updateIMU(float gx, float gy, float gz, float ax, float ay, float az);
	// float getRoll() {
	// 	return roll * 57.29578f;
	// }
	// float getPitch() {
	// 	return pitch * 57.29578f;
	// }
	// float getYaw() {
	// 	return yaw * 57.29578f + 180.0f;
	// }
	// float getRollRadians() {
	// 	return roll;
	// }
	// float getPitchRadians() {
	// 	return pitch;
	// }
	// float getYawRadians() {
	// 	return yaw;
	// }

	ATTITUDE_t getAttitude() {
		return {roll * 57.29578f, pitch * 57.29578f, yaw * 57.29578f + 180.0f};
	}
	ATTITUDE_t getAttitudeRadians() {
		return {roll, pitch, yaw};
	}
};

#endif
