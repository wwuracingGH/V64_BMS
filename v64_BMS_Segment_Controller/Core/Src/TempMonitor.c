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
 * Private function prototypes
 */
void tmSelectThermistor(uint8_t index);

/*
 * Public function declarations
 */

void TMInit(ADC_HandleTypeDef hadc){
	TMadc = hadc;
}

/*
 * Private function declaration
 */
void tmRead() {
	for( uint8_t i = 0; i < NUMTHERMISTORS; i++ ) {
		tmSelectThermistor(i);
		//TODO: Read ADC Values
	}
}

void tmSelectThermistor(uint8_t index) {
	uint8_t s0 = index&0x01;
	uint8_t s1 = index&0x02;
	uint8_t s2 = index&0x04;
	uint8_t s3 = index&0x08;

	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,s0);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,s1);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,s2);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,s3);
}
