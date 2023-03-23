/*	TempMonitor.c - Contains functionality for reading temperature values from thermistors
 * 	Thermistors are rated for 100c maximum temp
 *
 *  2022 Brandon Ramirez	-	V63
 *  2023 Brandon Ramirez	- 	V64
 */

#include "stm32g0xx_hal.h"
#include "TempMonitor.h"

/*****************************************************************************************
 * Module private variables
 ****************************************************************************************/
static ADC_HandleTypeDef *tmadc;
static float cellTemps[2][NUMTHERMISTORS] = {{0},{0}};	// Holds thermistor values in C
static uint8_t lowestTemp;
static uint8_t highestTemp;
static uint8_t avgTemp;

// Sin wave lookup table
static const int16_t tmCelciusADCVal[22] = { 4026, 3771, 3516, 3261, 3006, 2751, 2568,
												2394, 2202,	2014, 1848, 1690, 1496, 1345,
												1200, 1074, 970, 866, 784, 694, 630, 0 };

/*
 * Private function prototypes
 */
void tmSelect(uint8_t index);
float tmConvertToTemp(uint16_t adcVal);
float tmConvertCtoF(float celcius);

/*****************************************************************************************
* TMInit() - PUBLIC
*   parameters: Pointer to ADC Peripheral
*   return: none
*   description: Stores memory location of ADC peripheral
*****************************************************************************************/
void TMInit(ADC_HandleTypeDef *hadc1){
	tmadc = hadc1;
}

/*****************************************************************************************
* TMSampleTemps() - PUBLIC
*   parameters: none
*   return: none
*   description: Cycles through all thermistors sampling their ADC values and converts to
*   celcius
*****************************************************************************************/
void TMSampleTemps() {

	uint16_t rawtemps[2][NUMTHERMISTORS] = {{0}, {0}};
	uint16_t curtemp = 0;

	// Sample all ADC values and convert to C
	for( uint8_t i = 0; i < NUMTHERMISTORS; i++ ) {
		tmSelect(i);

		// Right side thermistors
		ADC_Select_CH0();
		HAL_ADC_Start(tmadc);
		HAL_ADC_PollForConversion(tmadc, TMADCTIMEOUT);
		curtemp = HAL_ADC_GetValue(tmadc);
		HAL_ADC_Stop(tmadc);

		rawtemps[0][i] = tmConvertToTemp(curtemp);

		// Left side thermistors
		ADC_Select_CH6();
		HAL_ADC_Start(tmadc);
		HAL_ADC_PollForConversion(tmadc, TMADCTIMEOUT);
		curtemp = HAL_ADC_GetValue(tmadc);
		HAL_ADC_Stop(tmadc);

		rawtemps[1][i] = tmConvertToTemp(curtemp);
	}

	// Convert from Celcius to Fahrenheit and store
	for( uint8_t i = 0; i < NUMTHERMISTORS; i++) {
		cellTemps[0][i] = tmConvertCtoF(rawtemps[0][i]);
		cellTemps[1][i] = tmConvertCtoF(rawtemps[1][i]);
	}
}

/*****************************************************************************************
* TMGetTemps() - PUBLIC
*   parameters: Array to fill
*   return: none
*   description: Takes pointer to array and fills it with current temp values
*****************************************************************************************/
void TMGetTemps(int16_t **temparray) {
	for(uint8_t i = 0; i < NUMTHERMISTORS; i++) {
		temparray[0][i] = cellTemps[0][i];
		temparray[1][i] = cellTemps[1][i];
	}
}

/*****************************************************************************************
* ADC_Select_CH0() - PUBLIC
*   parameters: none
*   return: none
*   description: Switches CH0 to active ADC channel
*****************************************************************************************/
void ADC_Select_CH0(void) {
	ADC_ChannelConfTypeDef sConfig = {0};
	sConfig.Channel = ADC_CHANNEL_0;
	sConfig.Rank = 1;
	if (HAL_ADC_ConfigChannel(tmadc, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
}

/*****************************************************************************************
* ADC_Select_CH6() - PUBLIC
*   parameters: none
*   return: none
*   description: Switches CH6 to active ADC channel
*****************************************************************************************/
void ADC_Select_CH6(void) {
	ADC_ChannelConfTypeDef sConfig = {0};
	sConfig.Channel = ADC_CHANNEL_6;
	sConfig.Rank = 1;
	if (HAL_ADC_ConfigChannel(tmadc, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
}

/*****************************************************************************************
* tmSelect() - PRIVATE
*   parameters: thermistor index
*   return: none
*   description: Selects given index on MUX
*****************************************************************************************/
void tmSelect(uint8_t index) {
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,index&0x01);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,index&0x02);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,index&0x04);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,index&0x08);
}

/*****************************************************************************************
* tmConvertToTemp() - PRIVATE
*   parameters: none
*   return: (float) Thermistor temperature in C
*   description: Converts given ADC value to temp based on calibration data from 3D printers
*   Linearly interpolates between measured ADC values at 5 degree (Celcius) intervals
*   Conversion to Fahrenheit is done later since all values were recorded in celcius during
*   calibration
*****************************************************************************************/
float tmConvertToTemp(uint16_t adcVal) {
	float tdif;
	float difn;

	uint8_t i = 0;
	while (adcVal < tmCelciusADCVal[i]){	// Find location in lookup table
		i++;
	}

	tdif = tmCelciusADCVal[i-1] - adcVal;	// Find difference between lower bound and value
	difn = tdif/(tmCelciusADCVal[i]-tmCelciusADCVal[i-1]);	// Find normalized difference between endpoints

	return (i-1)*TABLETEMPINT + (difn)*i*TABLETEMPINT;	// Calculate temperature by adding normalized difference
}

/*****************************************************************************************
* tmConvertCtoF() - PRIVATE
*   parameters: Temperature in celcius as float
*   return: (float) Thermistor temperature in F
*   description: Returns farenheight value of given celcius temperature
*****************************************************************************************/
float tmConvertCtoF(float celcius) {
	return celcius*1.8 + 32;
}

