//****************************************************
//
//	@file 		wheel_speed.c
//	@author 	Matthew Mora
//	@created	September 14th, 2023
//	@brief		Calculates wheel speed from wheel speed sensors
//
//****************************************************

#include "wheel_speed.h"
#include "cmsis_os.h"

// define statements
#define WHEEL_DIAMETER		18.1										// In inches
#define WHEEL_CIRCUMFERENCE (WHEEL_DIAMETER * PI * INCHES_TO_METERS)	// In meters
#define INCHES_TO_METERS	0.0254
#define PI					3.14159
#define NUM_TEETH			42							// number of teeth on rotary encoder

#define DELAY 500										// time in ms

static const double conversionFactor = (1/DELAY)*(1000.0);	// convert 1/ms to 1/s

extern volatile int wheel_FL_pulse_count;
extern volatile int wheel_FR_pulse_count;

// gets wheel speeds as m/s
double calculateWheelSpeed(int pulse_count){
	volatile double wheel_speed;

	wheel_speed = pulse_count*conversionFactor*WHEEL_CIRCUMFERENCE/NUM_TEETH;	// calculate wheel speed

	return wheel_speed;
}

void StartGetWheelSpeedTask(void const * argument){
	volatile double FL_speed;
	volatile double FR_speed;

	for (;;){
		osDelay(pdMS_TO_TICKS(DELAY));	// wait DELAY ms

		// calculates wheel speeds
		FL_speed = calculateWheelSpeed(wheel_FL_pulse_count);
		FR_speed = calculateWheelSpeed(wheel_FR_pulse_count);

		/* TODO SCU#35 */
		/* Logging Starts */

		/* Logging Ends */

		// reset counts
		wheel_FL_pulse_count = 0;
		wheel_FR_pulse_count = 0;
	}
}
