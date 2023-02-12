/*
 * MaxAFE.h
 *
 * 2022 Peter Schinske
 * 2023 Brandon Ramirez
 */

#ifndef SRC_MAXAFE_H_
#define SRC_MAXAFE_H_

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
#define T_LS_DELAY 25 //time to shift cap voltages to ground reference, in us
#define T_SET 5 //time to let a_out settle, in us
#define SPI_TIMEOUT 500
#define ADC_TIMEOUT 50

#define BYTE_COUNT 3
#define NUM_CELLS 8

/*
 * Public function declarations
 */
void MaxInit(SPI_HandleTypeDef *maxSPI, ADC_HandleTypeDef *maxADC, TIM_HandleTypeDef *maxHTIM);
void MaxSampleCharges();
void MaxDischargeCells(uint8_t cell);
void ADC_Select_CH0(void);
void ADC_Select_CH6(void);
void ADC_Select_CH7(void);

#endif /* SRC_MAXAFE_H_ */
