/*	TempMonitor.c
 *
 *  Created on: Jan 28, 2023
 *      Author: brandonramirez
 */

#include "stm32g0xx_hal.h"
#include "TempMonitor.h"

ADC_HandleTypeDef *tmadc1;
ADC_HandleTypeDef *tmadc2;

float cellTemps[NUMTHERMISTORS] = {0};

uint8_t segLowestTemp[6];
uint8_t segHighestTemp[6];
uint8_t segAvgTemp[6];
uint8_t avgCellTemp;
uint8_t highestCellTemp;

/*
 * Private function prototypes
 */
void tmSelectThermistor(uint8_t index);

/*
 * Public function declarations
 */

void TMInit(ADC_HandleTypeDef *hadc1, ADC_HandleTypeDef *hadc2){
	tmadc1 = hadc1;
	tmadc2 = hadc2;
}

/*
 * Private function declaration
 */
void tmRead() {

	uint16_t rawtemps[NUMTHERMISTORS] = {0};
	for( uint8_t i = 0; i < NUMTHERMISTORS; i++ ) {
		tmSelectThermistor(i);

		HAL_ADC_Start(tmadc1);
		//HAL_ADC_PollForConversion(tmadc1, ADC_TIMEOUT);
		rawtemps[i] = HAL_ADC_GetValue(tmadc1)/4096;
		HAL_ADC_Stop(tmadc1);

		HAL_ADC_Start(tmadc2);
		//HAL_ADC_PollForConversion(tmadc2, ADC_TIMEOUT);
		rawtemps[i+NUMTHERMISTORS] = HAL_ADC_GetValue(tmadc2)/4096;
		HAL_ADC_Stop(tmadc2);

		//cellTemps = tmConvertToTemps(rawtemps);
	}
}

void tmSelect(uint8_t index) {
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,index&0x01);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,index&0x02);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,index&0x04);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,index&0x08);
}
float *tmConvertToTemps(uint16_t temps[]) {
	static float celltemps[NUMTHERMISTORS] = {0};
	return celltemps;
}

uint16_t Read_ADC(uint32_t channel){

	ADC_ChannelConfTypeDef sConfig = {0};
	uint16_t adc_val;

	sConfig.Channel = channel;
	sConfig.Rank = 1;

if (HAL_ADC_ConfigChannel(tmadc1, &sConfig) != HAL_OK)
	{
	  Error_Handler();
	}

	HAL_ADC_Start(tmadc1); // start the adc
	HAL_ADC_PollForConversion(tmadc1, 100); // poll for conversion
	adc_val = HAL_ADC_GetValue(tmadc1); // get the adc value
	HAL_ADC_Stop(tmadc1); // stop adc

	sConfig.Rank = ADC_RANK_NONE;
	if (HAL_ADC_ConfigChannel(tmadc1, &sConfig) != HAL_OK) {
	Error_Handler();
	}

	return adc_val;
}

