/*
 * MaxFrontEnd.h - Contains functionality for interfacing with the MAX
 *
 * 2022 Peter Schinske	-	V63
 * 2023 Brandon Ramirez	-	V64
 */

#ifndef SRC_MAXAFE_H_
#define SRC_MAXAFE_H_

/*****************************************************************************************************
 * Definitions
 *****************************************************************************************************/
/*
 * Selection Bits
 */
#define ECS_HIGH 	0x80 //If on, enable cell selection
#define SC0_HIGH 	0x40 //Select cell for voltage readout during hold phase
#define SC1_HIGH 	0x20
#define SC2_HIGH 	0x10
#define SC3_HIGH 	0x08
#define SMPLB_HIGH 	0x04 //If on, in hold phase. Else, if sampl input high, then in sample phase
#define DIAG_HIGH 	0x02 //If on, in diagnostic mode (10uA sunk on all cell inputs)
#define LOPW_HIGH 	0x01 //If on, low power mode enabled

/*
 * Timings
 */
#define T_SAMPLE 40 //time to stay in sample phase, in ms
#define T_HOLD 1	//time to transition into hold phase, in us
#define T_LS_DELAY 30 //time to shift cap voltages to ground reference, in us
#define T_SET 5 //time to let a_out settle, in us
#define SPI_TIMEOUT 50
#define ADC_TIMEOUT 50

/*
 * Application specific
 */
#define BYTE_COUNT 3
#define NUM_CELLS 8
#define NUM_SWITCHES 2
/*****************************************************************************************
 * Public function declarations
 *****************************************************************************************/

/*****************************************************************************************
* MaxInit() - PUBLIC
*   parameters: SPI, ADC, and Hardware timer peripheral pointers
*   return: none
*   description: Save peripheral memory locations and reset config TxBuffer
*****************************************************************************************/
void MaxInit(SPI_HandleTypeDef *maxSPI, ADC_HandleTypeDef *maxADC, TIM_HandleTypeDef *maxHTIM);

/*****************************************************************************************
* MaxDischargeCell() - PUBLIC
*   parameters: Cell index
*   return: none
*   description: Enables the discharge circuit for the selected cell
*****************************************************************************************/
void MaxDischargeCell(uint8_t cell);

/*****************************************************************************************
* MaxDischargeTest() - PUBLIC
*   parameters: none
*   return: none
*   description: Cycle through discharging cells to test circuit and LEDs
*****************************************************************************************/
void MaxDischargeTest(void);

/*****************************************************************************************
* MaxSampleCharges() - PUBLIC
*   parameters: none
*   return: none
*   description: Takes MAX though sampling and hold states, read ADC values of cells
*****************************************************************************************/
void MaxSampleCharges(void);

#endif /* SRC_MAXAFE_H_ */
