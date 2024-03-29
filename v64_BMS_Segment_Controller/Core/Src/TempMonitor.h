/*
 * TempMonitor.h
 *
 *  2022 Brandon Ramirez	-	V63
 *  2023 Brandon Ramirez	-	V64
 */

#ifndef SRC_TEMPMONITOR_H_
#define SRC_TEMPMONITOR_H_

/*****************************************************************************************
 * Definitions
 ****************************************************************************************/
#define NUMTHERMISTORS 14	// Thermistors in single connector
#define TMADCTIMEOUT 100
#define VDD 3.3				// Supply voltage
#define TABLETEMPINT 5

/*****************************************************************************************
* TMInit() - PUBLIC
*   parameters: Pointer to ADC Peripheral
*   return: none
*   description: Stores memory location of ADC peripheral
*****************************************************************************************/
void TMInit(ADC_HandleTypeDef *hadc1);

/*****************************************************************************************
* TMSampleTemps() - PUBLIC
*   parameters: none
*   return: none
*   description: Cycles through all thermistors sampling their ADC values
*****************************************************************************************/
void TMSampleTemps(void);

/*****************************************************************************************
* TMGetTemps() - PUBLIC
*   parameters: Array to fill
*   return: none
*   description: Takes pointer to array and fills it with current temp values
*****************************************************************************************/
void TMGetTemps(int16_t **temparray);

/*****************************************************************************************
* ADC_Select_CH0() - PUBLIC
*   parameters: none
*   return: none
*   description: Switches CH0 to active ADC channel
*****************************************************************************************/
void ADC_Select_CH0(void);

/*****************************************************************************************
* ADC_Select_CH6() - PUBLIC
*   parameters: none
*   return: none
*   description: Switches CH6 to active ADC channel
*****************************************************************************************/
void ADC_Select_CH6(void);


#endif /* SRC_TEMPMONITOR_H_ */
