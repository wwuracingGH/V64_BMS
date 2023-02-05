/*
 * MaxAFE.c
 *
 *  Created on: Jan 28, 2023
 *      Author: brandonramirez
 */

#include "stm32g0xx_hal.h"
#include "MaxFrontEnd.h"

SPI_HandleTypeDef hspi;
ADC_HandleTypeDef hadc;
TIM_HandleTypeDef htim;

// SPI Transmission Buffer
uint8_t u1_buffer_in[] = { 0x00, 0x00, 0x80 };
// SPI Receiver buffer
uint32_t buffer_out[2] = { 0x01234567, 0x89ABCDEF};

// MAX SPI control buffer pointers
uint8_t *lower8 = &u1_buffer_in[0];
uint8_t *upper8 = &u1_buffer_in[1];
uint8_t *config = &u1_buffer_in[2];


// Voltage Data
float cell_voltages[8] = { 0 };
int lowest_cell = 0;
int highest_cell = 0;

/*
 * Private function definitions
 */

void maxInit(SPI_HandleTypeDef maxSPI, ADC_HandleTypeDef maxADC, TIM_HandleTypeDef maxHTIM) {
	hspi = maxSPI;
	hadc = maxADC;
	htim = maxHTIM;
}

void delay_us(uint16_t us) {
	__HAL_TIM_SET_COUNTER(&htim,0);
	while (__HAL_TIM_GET_COUNTER(&htim) < us);
}

// Takes integer and returns 8 bit big endian selection
uint8_t selectCell(uint8_t cellNum) {
	uint8_t selection = 0;
	// If even number
	if(cellNum%2 == 0) {
		selection |= SC0_HIGH;
	}
	if(cellNum == 3||4||7||8) {
		selection |= SC1_HIGH;
	}
	if(cellNum == 5||6||7||8) {
		selection |= SC2_HIGH;
	}
	return selection;
}

/*
 * Public function definitions
 */

void MaxSampleCharges(SPI_HandleTypeDef hspi) {


	//Empty transmission buffer and start sample phase
	*lower8 = 0;
	*upper8 = 0;
	*config = 0;
	HAL_SPI_Transmit(&hspi, u1_buffer_in, BYTE_COUNT, SPI_TIMEOUT);

	//wait for sample phase to complete, at least 40 ms
	HAL_Delay(SAMPLE_DELAY);

	//start hold phase
	*config = SMPLB_HIGH;
	HAL_SPI_Transmit(&hspi, u1_buffer_in, BYTE_COUNT, SPI_TIMEOUT);

	//wait for sample cap voltages to shift to ground reference, at least 50.5 us
	delay_us(HOLD_DELAY + LEVEL_SHIFT_DELAY);

	*lower8 = 0;
	*upper8 = 0;

	//Measure voltage of every set of cells
	for (int i = 0; i < 8; i++) {

		// Reset config register to only ECS_HIGH
		*config = ECS_HIGH;

		// Select desired cell
		*config |= selectCell(i);

		//tell MAX14920 to measure voltage of cell i
		HAL_SPI_Transmit(&hspi, u1_buffer_in, BYTE_COUNT, SPI_TIMEOUT);

		//Time delay to allow voltage measurement to settle.
		//According to MAX14920 datasheet, we should have a delay of over 5us.
		//Reading from ADC takes a few microseconds anyways.
		delay_us(10);

		//read voltage of cell i from ADC
		HAL_ADC_Start(&hadc);
		HAL_ADC_PollForConversion(&hadc, ADC_TIMEOUT);
		cell_voltages[i] = HAL_ADC_GetValue(&hadc) / 4096.0 * 15100 / 10000;
		if (cell_voltages[i] > cell_voltages[highest_cell]) {
			highest_cell = i;
		} else if (cell_voltages[i] < cell_voltages[lowest_cell]) {
			lowest_cell = i;
		}
	}

	/*
	buffer_out[0] = 12345687;//cell_voltages[lowest_cell];
	buffer_out[1] = 89012344;//cell_voltages[highest_cell];

	//send cell voltages to BMS controller
	HAL_SPI_Transmit(&hspi2, (uint8_t *) buffer_out, 8, SPI_TIMEOUT*50);
	continue;
	*/
}
