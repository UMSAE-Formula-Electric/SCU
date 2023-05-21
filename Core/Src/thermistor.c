#include "thermistor.h"
#include "math.h"
#include "stdio.h"
#include "adc.h"
#include <usart.h>
#include <stdio.h>
#include <string.h>

uint32_t res[16];

//TO-DO: Filter Noise
//use low pass filter at adc input,
//increase adc sampling time,
//isolate analog tracks on PCB from noisy signals,
//calibrate adc at startup,
//use digital filtering (DSP)

// STEINHART & HART Equation Coefficients
const double A = 1.2794639360 * pow(10, -3);
const double B = 2.6408831422 * pow(10, -4);
const double C = 1.3679771000 * pow(10, -7);

const double ADC_TO_Voltage = 3.3 / 4095;
const float vDD = 3.3;
const uint32_t constResistance = 1200;

double volatile temperatures[16];
double volatile naturalLogR;
double volatile temperature;
double volatile R_NTC;
volatile int newData = 0;

void get_NTC_Resistance(double voltageReading){
	if (voltageReading >= (vDD-0.1) || voltageReading <= 0){R_NTC = 0;}
	else {R_NTC = (vDD/voltageReading - 1) * constResistance;}
}

double getTemperature(double voltageReading){		// USING STEINHART & HART EQUATION
	get_NTC_Resistance(voltageReading);
	if (R_NTC < 1){temperature = -1;}
	else{
		naturalLogR = log(R_NTC);
		temperature = -273.15 + (1 / (A + B * naturalLogR + C * pow(naturalLogR, 3)));
	}
	return temperature;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	newData = 1;
}

void StartReadTempTask(void const * argument){
//	char msg[512];
	char tempMsg[50];

	for (;;){
		HAL_ADC_Start_DMA(&hadc1, res, 16);
		while(!newData) {}
		newData = 0;

		for(int i = 0; i < 16; i++) {
			  temperatures[i] = getTemperature(ADC_TO_Voltage * res[i]);
		}
		sprintf(tempMsg, "Temperature: %f\r\n", temperatures[1]);
		HAL_USART_Transmit(&husart1, (uint8_t *) tempMsg, strlen(tempMsg), 10);

//		SD_Log(msg, -1);
//		memset(msg, 0, sizeof msg);
		HAL_Delay(10);
	}
}
