/*
 * MaxAFE.h
 *
 *  Created on: Jan 28, 2023
 *      Author: brandonramirez
 */

#ifndef SRC_MAXAFE_H_
#define SRC_MAXAFE_H_

#define ECS_HIGH 	0x80 //If on, enable cell selection
#define SC0_HIGH 	0x40 //Select cell for voltage readout during hold phase
#define SC1_HIGH 	0x20
#define SC2_HIGH 	0x10
#define SC3_HIGH 	0x08
#define SMPLB_HIGH 	0x04 //If on, in hold phase. Else, if sampl input high, then in sample phase
#define DIAG_HIGH 	0x02 //If on, in diagnostic mode (10uA sunk on all cell inputs)
#define LOPW_HIGH 	0x01 //If on, low power mode enabled

#define SAMPLE_DELAY 45 //time to stay in sample phase, in ms
#define HOLD_DELAY 1	//time to transition into hold phase, in us
#define LEVEL_SHIFT_DELAY 55 //time to shift cap voltages to ground reference, in us
#define SETTLING_DELAY 50 //time to let a_out settle, in us

#endif /* SRC_MAXAFE_H_ */
