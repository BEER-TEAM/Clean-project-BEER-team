/*
 * door_lock.h
 *
 *  Created on: Sep 24, 2024
 *      Author: gangm
 */

#ifndef DOOR_LOCK_DOOR_LOCK_H_
#define DOOR_LOCK_DOOR_LOCK_H_
#include "main.h"
#include "gpio.h"

typedef enum
{
	DOORS_CLOSED =0,
	DOORS_OPEN =1,
}door_state_enum;

static door_state_enum door_state;

void door_open(void);
void door_close(void);

void door_maintain(void);

#endif /* DOOR_LOCK_DOOR_LOCK_H_ */
