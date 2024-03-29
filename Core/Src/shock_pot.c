//********************************************************************
//
//	@file 		shock_pot.c
//	@author 	Matthew Mora
//	@created	Nov 19, 2022
//	@brief		Calculates flowrate from flowmeters
//
//	@datasheet	https://drive.google.com/file/d/1g9wjH6BT5--y21_IYlu2G4MbX3KbiAo5/view?usp=share_link
//
//*********************************************************************

#include "shock_pot.h"
#include "stdio.h"
#include "adc.h"
#include "cmsis_os.h"
#include <usart.h>
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"

// variables defined in thermistor.c
extern const double ADC_TO_Voltage;
extern const float vDD;

const float MAX_DIST = 50;	// max travel of shock potentiometer in mm
volatile double dist[16];	// holds distances read from each ADC input (note: not all inputs are necessarily connected to a shock pot

//*********************************************************************
// getDistance
//
// PURPOSE: This function translates voltages to distance in millimeters (mm)
//
// INPUT PARAMTERS:
//			voltage - voltage measured across the shock pot
//
// RETURN:	distance in mm of type double
//*********************************************************************
double getDistance(double voltage){
	double dist = MAX_DIST * voltage / vDD;
	return dist;
}

//*********************************************************************
// readDist_task
//
// PURPOSE: Main loop for freeRTOS thread. Waits for ADC conversion and
//			translates voltages into distance
//*********************************************************************
void StartReadDistTask(void const * argument){
	char msg[512];
	char msgDist[20];
	float voltages[16];
	char distMsg[50];

	for (;;){
		if (newData_shock_pot == 1){
			// calculate distances for each ADC channel
			for(int i = 0; i < 16; i++) {
				  voltages[i] = ADC_TO_Voltage * ADC_Readings[i];
				  dist[i] = getDistance(voltages[i]);
				  sprintf(msgDist, "ADC %d %.5f \n", i, voltages[i]);
				  strcat(msg,msgDist);
			}

			/* TODO SCU#35 */
			/* Logging Starts */
			// add ADC channel 0 to message
			sprintf(distMsg, "Distance: %f\r\n", dist[0]);
			HAL_USART_Transmit(&husart1, (uint8_t *) distMsg, strlen(distMsg), 10);
			/* Logging Ends */

			newData_shock_pot = 0;					// reset ADC conversion flag
		}

		// wait 500ms
		osDelay(500);
	}
}
