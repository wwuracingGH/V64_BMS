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
uint8_t u1_buffer_in[] = { 0x00, 0x00, 0x00 };
uint8_t status_buffer_in[] = { 0x00, 0x00, 0x00 };

// MAX SPI control buffer pointers
uint8_t *balanceLower = &u1_buffer_in[0];
uint8_t *balanceUpper = &u1_buffer_in[1];
uint8_t *config = &u1_buffer_in[2];


// Voltage Data
float cell_voltages[NUM_CELLS] = { 0 };
uint8_t lowest_cell = 0;
uint8_t highest_cell = 0;

/*
 * Private function definitions
 */


void delay_us(uint16_t us) {
	__HAL_TIM_SET_COUNTER(maxtim,0);
	while (__HAL_TIM_GET_COUNTER(maxtim) < us);
}

void MaxInit(SPI_HandleTypeDef *hspi, ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim) {
	maxspi = hspi;
	maxadc = hadc;
	maxtim = htim;
	*balanceLower = 0;
	*balanceUpper = 0;
	*config = 0;
}

// Takes 4-bit integer and returns 4-bit big endian selection
uint8_t selectCell(uint8_t cellNum) {
	uint8_t selection = 0;
	uint8_t bit0 = ((cellNum>>3) &0x01);
	uint8_t bit1 = ((cellNum>>2) &0x01);
	bit1 <<=1;
	uint8_t bit2 = ((cellNum>>1) &0x01);
	bit2 <<=2;
	uint8_t bit3 = ((cellNum) & 0x01);
	bit3 <<=3;

	selection = bit3 | bit2 | bit1 | bit0;
	return selection;

}

/*
 * Public function definitions
 */
void MaxDischargeCells(uint8_t cell) {
	//for(uint8_t i = 0; i < NUM_CELLS; i++) {
	//}
	// Test Discharge LEDs
	*balanceLower = 0x01;
	*balanceUpper = 0x01;
	*config = 0x01;
	HAL_SPI_Transmit(maxspi, u1_buffer_in, BYTE_COUNT, SPI_TIMEOUT);

}

void MaxSampleCharges() {
	//Empty transmission buffer and start sample phase
	*balanceLower = 0;
	*balanceUpper = 0;
	*config = 0;
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(maxspi, u1_buffer_in, status_buffer_in, BYTE_COUNT, SPI_TIMEOUT);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_SET);

	//wait for sample phase to complete, at least 40 ms
	HAL_Delay(T_SAMPLE);

	//start hold phase
	*config = SMPLB_HIGH;
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(maxspi, u1_buffer_in, status_buffer_in, BYTE_COUNT, SPI_TIMEOUT);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_SET);

	//wait for sample cap voltages to shift to ground reference, at least 50.5 us
	delay_us(T_HOLD + T_LS_DELAY);

	//Measure voltage of every set of cells
	for (int i = 0; i < NUM_CELLS; i++) {

		// Reset config register to only ECS and SAMPLB
		*config = ECS_HIGH | SMPLB_HIGH;

		// Select desired cell
		*config |= (selectCell(i)<<3);

		//tell MAX14920 to measure voltage of cell i
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_RESET);
		HAL_SPI_TransmitReceive(maxspi, u1_buffer_in, status_buffer_in, BYTE_COUNT, SPI_TIMEOUT);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_SET);

		/*
		*config = SC2_HIGH | SC3_HIGH;

		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_RESET);
		HAL_SPI_TransmitReceive(maxspi, u1_buffer_in, status_buffer_in, BYTE_COUNT, SPI_TIMEOUT);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_SET);

		*/
		//Time delay to allow voltage measurement to settle.
		//According to MAX14920 datasheet, we should have a delay of over 5us.
		//Reading from ADC takes a few microseconds anyways.
		delay_us(T_SET);

		//read voltage of cell i from ADC
		HAL_ADC_Start(maxadc);
		HAL_ADC_PollForConversion(maxadc, ADC_TIMEOUT);
		cell_voltages[i] = HAL_ADC_GetValue(maxadc)/4096;
		HAL_ADC_Stop(maxadc);
		if (cell_voltages[i] > cell_voltages[highest_cell]) {
			highest_cell = i;
		} else if (cell_voltages[i] < cell_voltages[lowest_cell]) {
			lowest_cell = i;
		}
	}
}
