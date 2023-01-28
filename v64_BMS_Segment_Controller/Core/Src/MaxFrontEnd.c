/*
 * MaxAFE.c
 *
 *  Created on: Jan 28, 2023
 *      Author: brandonramirez
 */

SPI_HandleTypeDef hspi;
ADC_HandleTypeDef hadc;

uint8_t u1_buffer_in[] = { 0x00, 0x00, 0x80 };
uint8_t *balance_low = &u1_buffer_in[0];
uint8_t *balance_high = &u1_buffer_in[1];
uint8_t *config = &u1_buffer_in[2];

float cell_voltages[4] = { 0 };

uint32_t buffer_out[2] = { 0x01234567, 0x89ABCDEF};

void maxInit() {

}

//wait for a given number of microseconds
//taken from https://controllerstech.com/create-1-microsecond-delay-stm32/
void delay_us(uint16_t us) {
	//set the counter value to 0
	__HAL_TIM_SET_COUNTER(&htim14,0);

	//wait for the counter to reach the given microsecond value
	//we've configured the timer to tick once every microsecond, so we don't have to weight the values
	while (__HAL_TIM_GET_COUNTER(&htim14) < us);
}

void MaxSampleCharges() {

	//charge up sample capacitors
	*balance_low = 0;
	*balance_high = 0;
	*config = 0;

	//start sample phase - charge up sample capacitors
	HAL_SPI_Transmit(&hspi, u1_buffer_in, 3, SPI_TIMEOUT);

	//wait for sample phase to complete, at least 40 ms
	HAL_Delay(SAMPLE_DELAY);

	//start hold phase
	*config = SMPLB_HIGH;
	HAL_SPI_Transmit(&hspi, u1_buffer_in, 3, SPI_TIMEOUT);

	//wait for sample cap voltages to shift to ground reference, at least 50.5 us
	delay_us(HOLD_DELAY + LEVEL_SHIFT_DELAY);

	*balance_low = 0;
	*balance_high = 0;
	//Measure voltage of every set of cells
	int lowest_cell = 0;
	int highest_cell = 0;
	for (int i = 0; i < 4; i++) {
		*config = ECS_HIGH;
		//Set SC0 and SC1, depending on the value of i
		//Since we're only measuring 4 cells, SC2 and SC3 are always 0
		int cell = i;
		if (i / 2 != 0) {
			cell -= 2;
			*config = *config || SC1_HIGH;
		}
		if (cell == 1) {
			*config = *config || SC0_HIGH;
		}
		//tell MAX14920 to measure voltage of cell i
		HAL_SPI_Transmit(&hspi1, u1_buffer_in, 3, SPI_TIMEOUT);

		//Time delay to allow voltage measurement to settle.
		//According to MAX14920 datasheet, we should have a delay of over 5us.
		//Reading from ADC takes a few microseconds anyways.

		//read voltage of cell i from ADC
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 2);
		cell_voltages[i] = HAL_ADC_GetValue(&hadc1) / 4096.0 * 15100 / 10000;
		if (cell_voltages[i] > cell_voltages[highest_cell]) {
			highest_cell = i;
		} else if (cell_voltages[i] < cell_voltages[lowest_cell]) {
			lowest_cell = i;
		}
	}

	buffer_out[0] = 12345687;//cell_voltages[lowest_cell];
	buffer_out[1] = 89012344;//cell_voltages[highest_cell];

	//send cell voltages to BMS controller
	HAL_SPI_Transmit(&hspi2, (uint8_t *) buffer_out, 8, SPI_TIMEOUT*50);
	continue;
}
