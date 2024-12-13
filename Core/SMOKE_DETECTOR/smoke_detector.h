/*
 * smoke_detector.h
 *
 *  Created on: Sep 17, 2024
 *      Author: gangm
 */

#ifndef SMOKE_DETECTOR_SMOKE_DETECTOR_H_
#define SMOKE_DETECTOR_SMOKE_DETECTOR_H_

#include "gpio.h"
#include "stdint.h"
uint8_t is_smoke_detected(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

#endif /* SMOKE_DETECTOR_SMOKE_DETECTOR_H_ */
