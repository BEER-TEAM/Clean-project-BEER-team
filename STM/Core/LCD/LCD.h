/*
 * LCD.h
 *
 *  Created on: Sep 17, 2024
 *      Author: gangm
 */

#ifndef LCD_LCD_H_
#define LCD_LCD_H_

#include "stdint.h"
#include "i2c.h"

#define RS_PIN 0x01
#define RW_PIN 0x02
#define EN_PIN 0x04
#define BL_PIN 0x08

#define INIT_8_BIT_MODE	0x30
#define INIT_4_BIT_MODE	0x02

#define CLEAR_LCD	0x01

#define UNDERLINE_OFF_BLINK_OFF		0x0C
#define UNDERLINE_OFF_BLINK_ON		0x0D
#define UNDERLINE_ON_BLINK_OFF		0x0E
#define UNDERLINE_ON_BLINK_ON		0x0F

#define FIRST_CHAR_LINE_1	0x80
#define FIRST_CHAR_LINE_2	0xC0
#define FIRST_CHAR_LINE_3   0x90
#define FIRST_CHAR_LINE_4   0xD0

struct lcd_disp {
	I2C_HandleTypeDef * I2Chandler;
	uint8_t addr;
	char line_1[17];
	char line_2[17];
	char line_3[17];
	char line_4[17];
	uint8_t bl;
};



void lcd_init(struct lcd_disp * lcd);

void lcd_write(I2C_HandleTypeDef * I2Chandler, uint8_t addr, uint8_t data, uint8_t xpin);

void lcd_display(struct lcd_disp * lcd);
void lcd_clear(struct lcd_disp * lcd);


#endif /* LCD_LCD_H_ */
