/*
 * LCD.c
 *
 *  Created on: Sep 17, 2024
 *      Author: gangm
 */
#include "LCD.h"
#include "main.h"



void lcd_init(struct lcd_disp * lcd)
{
	uint8_t xpin = 0;
	/* set backlight */
	if(lcd->bl)
	{
		xpin = BL_PIN;
	}

	/* init sequence */
	HAL_Delay(40);
	lcd_write(lcd->I2Chandler,lcd->addr, INIT_8_BIT_MODE, xpin);
	HAL_Delay(5);
	lcd_write(lcd->I2Chandler,lcd->addr, INIT_8_BIT_MODE, xpin);
	HAL_Delay(1);
	lcd_write(lcd->I2Chandler,lcd->addr, INIT_8_BIT_MODE, xpin);

	/* set 4-bit mode */
	lcd_write(lcd->I2Chandler,lcd->addr, INIT_4_BIT_MODE, xpin);

	/* set cursor mode */
	lcd_write(lcd->I2Chandler,lcd->addr, UNDERLINE_OFF_BLINK_OFF, xpin);

	/* clear */
	lcd_clear(lcd);

}

void lcd_write(I2C_HandleTypeDef * I2Chandler, uint8_t addr, uint8_t data, uint8_t xpin)
{
	uint8_t tx_data[4];

	/* split data */
	tx_data[0] = (data & 0xF0) | EN_PIN | xpin;
	tx_data[1] = (data & 0xF0) | xpin;
	tx_data[2] = (data << 4) | EN_PIN | xpin;
	tx_data[3] = (data << 4) | xpin;

	/* send data via i2c */
	HAL_I2C_Master_Transmit(I2Chandler, addr, tx_data, 4, 100);

	HAL_Delay(5);
}

void lcd_display(struct lcd_disp * lcd)
{
	uint8_t xpin = 0, i = 0;

	/* set backlight */
	if(lcd->bl)
	{
		xpin = BL_PIN;
	}

	lcd_clear(lcd);

	/* send first line data */
	lcd_write(lcd->I2Chandler,lcd->addr, FIRST_CHAR_LINE_1, xpin);
	while(lcd->line_1[i])
	{
		lcd_write(lcd->I2Chandler,lcd->addr, lcd->line_1[i], (xpin | RS_PIN));
		i++;
	}

	/* send second line data */
	i = 0;
	lcd_write(lcd->I2Chandler,lcd->addr, FIRST_CHAR_LINE_2, xpin);
	while(lcd->line_2[i])
	{
		lcd_write(lcd->I2Chandler,lcd->addr, lcd->line_2[i], (xpin | RS_PIN));
		i++;
	}

	i = 0;
	lcd_write(lcd->I2Chandler,lcd->addr, FIRST_CHAR_LINE_3, xpin);
	while(lcd->line_3[i])
	{
		lcd_write(lcd->I2Chandler,lcd->addr, lcd->line_3[i], (xpin | RS_PIN));
		i++;
	}

	i = 0;
	lcd_write(lcd->I2Chandler,lcd->addr, FIRST_CHAR_LINE_4, xpin);
	while(lcd->line_4[i])
	{
		lcd_write(lcd->I2Chandler,lcd->addr, lcd->line_4[i], (xpin | RS_PIN));
		i++;
	}
}

void lcd_clear(struct lcd_disp * lcd)
{
	uint8_t xpin = 0;

	/* set backlight */
	if(lcd->bl)
	{
		xpin = BL_PIN;
	}

	/* clear display */
	lcd_write(lcd->I2Chandler,lcd->addr, CLEAR_LCD, xpin);
}
