/*
 * DHT11lib.c
 *
 *  Created on: Nov 8, 2025
 *      Author: weron
 */

#include "stm32f3xx_hal.h"


extern TIM_HandleTypeDef htim6;

void DHT11_SetPinOutput(GPIO_TypeDef *GPIOport, uint16_t GPIOPin)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIOPin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;// Tryb wyjścia push-pull
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;  // Niska prędkość (wystarczy dla DHT11)

    HAL_GPIO_Init(GPIOport, &GPIO_InitStruct);
}

void DHT11_SetPinInput(GPIO_TypeDef *GPIOport, uint16_t GPIOPin)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIOPin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;  // Niska prędkość (wystarczy dla DHT11)

    HAL_GPIO_Init(GPIOport, &GPIO_InitStruct);
}


