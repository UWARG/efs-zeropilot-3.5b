#include <cstdint>
#include <algorithm>

#define SBUS_INPUT_CHANNELS	16

/*  a struct for control signal channel mapping and attribute values*/
/*  for now, the value range is 0 to 100 float*/
typedef struct{
	float ControlSignals[SBUS_INPUT_CHANNELS];
	bool isDataNew;
	
	float &roll = ControlSignals[0];
	float &pitch = ControlSignals[1];
	float &throttle = ControlSignals[2];
	float &yaw = ControlSignals[3];
	float &arm = ControlSignals[4];
	float &aux1 = ControlSignals[5];
	float &aux2 = ControlSignals[6];
	float &aux3 = ControlSignals[7];
	float &aux4 =  ControlSignals[8];
	float &aux5 = ControlSignals[9];
	float &aux6 = ControlSignals[10];
	float &aux7 = ControlSignals[11];
	float &aux8 = ControlSignals[12];
	float &aux9 = ControlSignals[13];
	float &aux10 = ControlSignals[14];
	float &aux11 = ControlSignals[15];

	float &operator[] (int i) { return ControlSignals[i]; }

	RCControl_t operator=(const RCControl_t& other){
		std::copy(other.ControlSignals, other.ControlSignals + SBUS_INPUT_CHANNELS, this->ControlSignals);
		return *(RCControl_t*)this;
	}

	/*  initial values*/
	RCControl_t RCControl(){
		bool isDataNew;
		roll = 50.0f;
		pitch = 50.0f;
		throttle = 0.0f;
		yaw = 50.0f;
		arm = 0.0f;
		aux1 = 0.0f;
		aux2 = 0.0f;
		aux3 = 0.0f;
		aux4 = 0.0f;
		aux5 = 0.0f;
		aux6 = 0.0f;
		aux7 = 0.0f;
		aux8 = 0.0f;
		aux9 = 0.0f;
		aux10 = 0.0f;
		aux11 = 0.0f;
	}
} RCControl_t;