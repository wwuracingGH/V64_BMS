/*
 * TempMonitor.h
 *
 *  2022 Brandon Ramirez
 *  2023 Brandon Ramirez
 */

#ifndef SRC_TEMPMONITOR_H_
#define SRC_TEMPMONITOR_H_

#define NUMTHERMISTORS 14
#define ADC_TIMEOUT 10

/*
 * Public Variables
 */

/*
 * Private function declarations
 */

/*
 * Public function declarations
 */
void TMInit(ADC_HandleTypeDef *hadc1, ADC_HandleTypeDef *hadc2);


#endif /* SRC_TEMPMONITOR_H_ */
