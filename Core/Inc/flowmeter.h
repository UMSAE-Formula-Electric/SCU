//***************************
//
//	@file 		flowmeter.h
//	@author 	Matthew Mora
//	@created	January 2nd, 2023
//	@brief		Calculates flowrate from flowmeters
//
//***************************

#include "tim.h"
#include "FreeRTOS.h"
#include "task.h"

// avoid defining multiple times
#ifndef _FLOWMETER
#define _FLOWMETER

// function prototypes
// ***TODO*** SHOULD THIS REALLY BE IN THE FLOWMETER CODE?
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);	// alter Capture Callback to increment pulse counter

void StartGetFlowrateTask(void const * argument);			// Starts the task to get the flowrate from flowmeter(s)


#endif // ifndef _FLOWMETER
