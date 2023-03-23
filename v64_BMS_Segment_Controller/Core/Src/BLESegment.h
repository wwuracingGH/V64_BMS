/*
 * BLESegment.h
 *
 *  Created on: Feb 11, 2023
 *      Author: brandonramirez
 */

#ifndef SRC_BLESEGMENT_H_
#define SRC_BLESEGMENT_H_

/*****************************************************************************************
* I2C_EV_IRQHandler() - PUBLIC
*   parameters: none
*   return: none
*   description: Is called on interrupt from bluetooth
*****************************************************************************************/
void I2C_EV_IRQHandler(I2C_HandleTypeDef *hi2c);

/*****************************************************************************************
* BLESegmentInit() - PUBLIC
*   parameters: Pointer to I2C peripheral
*   return: none
*   description: Initializes I2C peripheral
*****************************************************************************************/
void BLESegmentInit(I2C_HandleTypeDef *i2c);

#endif /* SRC_BLESEGMENT_H_ */
