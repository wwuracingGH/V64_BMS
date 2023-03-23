/*
 * BLESegment.c
 *
 *  Created on: Feb 11, 2023
 *      Author: brandonramirez
 */

#include "stm32g0xx_hal.h"

I2C_HandleTypeDef *i2c;
static uint16_t devaddress = 0;
static uint8_t pData[] = {'h', 'e', 'l', 'l', 'o'};
static uint8_t Size;

/*****************************************************************************************
* BLESegmentInit() - PUBLIC
*   parameters: Pointer to I2C peripheral
*   return: none
*   description: Initializes I2C peripheral
*****************************************************************************************/
void BLESegmentInit(I2C_HandleTypeDef *hi2c) {
	i2c = hi2c;

	if(HAL_I2C_EnableListen_IT(i2c) != HAL_OK)
	{
		/* Transfer error in reception process */
		Error_Handler();
	}
}

/*****************************************************************************************
* I2C_EV_IRQHandler() - PUBLIC
*   parameters: none
*   return: none
*   description: Is called on interrupt from bluetooth
*****************************************************************************************/
void I2C_EV_IRQHandler(I2C_HandleTypeDef *i2c) {
	HAL_I2C_Master_Transmit_IT (i2c, devaddress, pData, sizeof(pData)/sizeof(pData[0]));
}
