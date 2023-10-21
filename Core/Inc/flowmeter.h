//***************************
//
//	@file 		flowmeter.h
//	@author 	Matthew Mora
//	@created	January 2nd, 2023
//	@brief		Calculates flowrate from flowmeters
//
//***************************


// avoid defining multiple times
#ifndef _FLOWMETER
#define _FLOWMETER

// function prototypes

void StartGetFlowrateTask(void const * argument);			// Starts the task to get the flowrate from flowmeter(s)


#endif // ifndef _FLOWMETER
