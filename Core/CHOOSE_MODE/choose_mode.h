/*
 * choose_mode.h
 *
 *  Created on: Sep 27, 2024
 *      Author: gangm
 */

#ifndef CHOOSE_MODE_CHOOSE_MODE_H_
#define CHOOSE_MODE_CHOOSE_MODE_H_


#define MODE_CHOICE_PERIOD 5000


typedef enum
{
	AES_HMAC_FSK = 0,
	AES_HMAC_LORA = 1,
	WEAK_STREAM_FSK = 2,
	WEAK_STREAM_LORA = 3,
	LACK_SECURITY_FSK = 4,
	LACK_SECURITY_LORA = 5,
	LACK_SECURITY_OOK = 6,
	WEAK_STREAM_OOK = 7,
	AES_HMAC_OOK = 8

}OPERATING_MODE;

OPERATING_MODE get_mode(void);

void display_mode(OPERATING_MODE opermode);



#endif /* CHOOSE_MODE_CHOOSE_MODE_H_ */
