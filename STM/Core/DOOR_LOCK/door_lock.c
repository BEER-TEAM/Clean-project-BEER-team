/*
 * door_lock.c
 *
 *  Created on: Sep 24, 2024
 *      Author: gangm
 */


#include "door_lock.h"

void door_open(void)
{
	door_state = DOORS_OPEN;
	HAL_GPIO_WritePin(DOOR_LED_GPIO_Port, DOOR_LED_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(DOOR_LED_CLOSED_GPIO_Port, DOOR_LED_CLOSED_Pin, GPIO_PIN_RESET);
}
void door_close(void)
{
	door_state = DOORS_CLOSED;
	HAL_GPIO_WritePin(DOOR_LED_CLOSED_GPIO_Port, DOOR_LED_CLOSED_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(DOOR_LED_GPIO_Port, DOOR_LED_Pin, GPIO_PIN_RESET);
}


void door_maintain(void)
{

	if(door_state == DOORS_OPEN)
	{
		door_open();
	}
	else
	{
		door_close();
	}
}
