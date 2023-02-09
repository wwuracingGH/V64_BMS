/*
 * MaxAFE.c
 *
 *  Created on: Jan 28, 2023
 *      Author: brandonramirez
 */

#include "stm32g0xx_hal.h"
#include "MaxFrontEnd.h"

SPI_HandleTypeDef *maxspi;
ADC_HandleTypeDef *maxadc;
TIM_HandleTypeDef *maxtim;

// SPI Transmission Buffer
uint8_t u1_buffer_in[] = { 0x00, 0x00, 0x80 };

// MAX SPI control buffer pointers
uint8_t *balanceLower = &u1_buffer_in[0];
uint8_t *balanceUpper = &u1_buffer_in[1];
uint8_t *config = &u1_buffer_in[2];


// Voltage Data
float cell_voltages[8] = { 0 };
uint8_t lowest_cell = 0;
uint8_t highest_cell = 0;

/*
 * Private function definitions
 */

void MaxInit(SPI_HandleTypeDef hspi, ADC_HandleTypeDef hadc, TIM_HandleTypeDef htim) {
	maxspi = &hspi;
	maxadc = &hadc;
	maxtim = &htim;
}

void delay_us(uint16_t us) {
	__HAL_TIM_SET_COUNTER(maxtim,0);
	while (__HAL_TIM_GET_COUNTER(maxtim) < us);
}

// Takes integer and returns 8 bit big endian selection
uint8_t selectCell(uint8_t cellNum) {
	uint8_t clone0 = cellNum >> 2;
	uint8_t clone2 = (cellNum << 2) & 0x4;
	uint8_t return_cell = (cellNum & 0x2)| clone0 | clone2;
	return return_cell;
}

/*
 * Public function definitions
 */
void MaxDischargeCells(uint8_t cell) {
	//for(uint8_t i = 0; i < NUM_CELLS; i++) {
	//}
	// Test Discharge LEDs
	*balanceLower = 0xFF;
	*balanceUpper = 0xFF;
	*config = 0;
	HAL_SPI_Transmit(maxspi, u1_buffer_in, BYTE_COUNT, SPI_TIMEOUT);

}

void MaxSampleCharges() {
	//Empty transmission buffer and start sample phase
	*balanceLower = 0;
	*balanceUpper = 0;
	*config = 0;
	HAL_SPI_Transmit(maxspi, u1_buffer_in, BYTE_COUNT, SPI_TIMEOUT);

	//wait for sample phase to complete, at least 40 ms
	HAL_Delay(SAMPLE_DELAY);

	//start hold phase
	*config = SMPLB_HIGH;
	HAL_SPI_Transmit(maxspi, u1_buffer_in, BYTE_COUNT, SPI_TIMEOUT);

	//wait for sample cap voltages to shift to ground reference, at least 50.5 us
	delay_us(HOLD_DELAY + LEVEL_SHIFT_DELAY);
	//HAL_Delay(10);
	*balanceLower = 0;
	*balanceUpper = 0;

	//Measure voltage of every set of cells
	for (int i = 0; i < 8; i++) {

		// Reset config register to only ECS_HIGH
		*config = ECS_HIGH;

		// Select desired cell
		*config |= selectCell(i);

		//tell MAX14920 to measure voltage of cell i
		HAL_SPI_Transmit(maxspi, u1_buffer_in, BYTE_COUNT, SPI_TIMEOUT);

		//Time delay to allow voltage measurement to settle.
		//According to MAX14920 datasheet, we should have a delay of over 5us.
		//Reading from ADC takes a few microseconds anyways.
		delay_us(6);
		//HAL_Delay(10);

		//read voltage of cell i from ADC
		ADC_Select_CH7();
		HAL_ADC_Start(maxadc);
		HAL_ADC_PollForConversion(maxadc, ADC_TIMEOUT);
		cell_voltages[i] = HAL_ADC_GetValue(maxadc) / 4096.0 * 15100 / 10000;
		HAL_ADC_Stop(maxadc);
		if (cell_voltages[i] > cell_voltages[highest_cell]) {
			highest_cell = i;
		} else if (cell_voltages[i] < cell_voltages[lowest_cell]) {
			lowest_cell = i;
		}
	}
}

void ADC_Select_CH0(void) {
	ADC_ChannelConfTypeDef sConfig = {0};
	sConfig.Channel = ADC_CHANNEL_0;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
	if (HAL_ADC_ConfigChannel(maxadc, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
}
 void ADC_Select_CH6(void) {
	ADC_ChannelConfTypeDef sConfig = {0};
	sConfig.Channel = ADC_CHANNEL_6;
	sConfig.Rank = ADC_REGULAR_RANK_2;
	if (HAL_ADC_ConfigChannel(maxadc, &sConfig) != HAL_OK)
	{
	 Error_Handler();
	}
 }
void ADC_Select_CH7(void) {
	ADC_ChannelConfTypeDef sConfig = {0};
	sConfig.Channel = ADC_CHANNEL_7;
	sConfig.Rank = ADC_REGULAR_RANK_3;
	if (HAL_ADC_ConfigChannel(maxadc, &sConfig) != HAL_OK)
	{
	Error_Handler();
	}
}
