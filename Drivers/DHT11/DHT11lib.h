/*
 * DHT11lib.h
 *
 *  Created on: Nov 8, 2025
 *      Author: weron
 */
#include "stm32f3xx_hal.h"

#ifndef DHT11_DHT11LIB_H_
#define DHT11_DHT11LIB_H_



void DHT11_SetPinOutput(GPIO_TypeDef *GPIOport, uint16_t GPIOPin);
void DHT11_SetPinInput(GPIO_TypeDef *GPIOport, uint16_t GPIOPin);



#endif /* DHT11_DHT11LIB_H_ */
