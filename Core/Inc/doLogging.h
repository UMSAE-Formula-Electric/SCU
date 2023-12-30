//***************************
//
//	@file 		doLogging.h
//	@author 	Breanna Stratton
//	@created	November 12th, 2023
//	@brief		Manage logging settings to control output to SD card and/or USART.
//
//***************************

#ifndef _CONTROLLED_LOGGING
#define _CONTROLLED_LOGGING

// Define LoggingModes
enum LoggingMode {
	logNeither = 0,
    logUSART,
    logSD,
    logBoth
};

//declare the external variable because it will be referred to in other files
extern enum LoggingMode loggingMode;

#endif


