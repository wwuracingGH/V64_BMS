/*
 * DigitalIsoComm.h
 *
 *  Created on: Mar 23, 2023
 *      Author: brandonramirez
 */

#ifndef SRC_DIGITALISOCOMM_H_
#define SRC_DIGITALISOCOMM_H_

/*****************************************************************************************
* DigInit() - PUBLIC
*   parameters: Pointer to SPI peripheral
*   return: none
*   description: Initializes SPI peripheral
*****************************************************************************************/
void DigInit(SPI_HandleTypeDef *hspi);

#endif /* SRC_DIGITALISOCOMM_H_ */
