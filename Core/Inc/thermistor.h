/*
 * thermistor.h
 *
 *  Created on: Aug 6, 2022
 *      Author: tonyz
 */

#include "adc.h"

#ifndef THERMISTOR_H_
#define THERMISTOR_H_


void StartReadTempTask(void const * argument);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);

#endif /* THERMISTOR_H_ */
