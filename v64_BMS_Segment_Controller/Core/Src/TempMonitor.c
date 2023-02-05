/*
 * TempMonitor.c
 *
 *  Created on: Jan 28, 2023
 *      Author: brandonramirez
 */

#include "stm32g0xx_hal.h"
#include "TempMonitor.h"

ADC_HandleTypeDef TMadc;

/*
 * Public function declarations
 */

void TMInit(ADC_HandleTypeDef hadc);
void TMSelectThermistor(uint8_t index);

