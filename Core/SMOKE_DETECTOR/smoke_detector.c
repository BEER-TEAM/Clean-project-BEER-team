/*
 * smoke_detector.c
 *
 *  Created on: Sep 17, 2024
 *      Author: gangm
 */
#include "smoke_detector.h"

uint8_t is_smoke_detected(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	return HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);
}

