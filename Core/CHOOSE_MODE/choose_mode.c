/*
 * choose_mode.c
 *
 *  Created on: Sep 27, 2024
 *      Author: gangm
 */

#include "choose_mode.h"
#include "main.h"
#include "gpio.h"


OPERATING_MODE get_mode(void)
{
	uint32_t time = HAL_GetTick();

	GPIO_PinState buttonState;
	GPIO_PinState lastButtonState;

	uint32_t buttonPressCount = 0;

	lastButtonState = HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin);

	while(HAL_GetTick()-time<MODE_CHOICE_PERIOD)
	{

        // Odczytanie aktualnego stanu przycisku
        buttonState = HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin);

        // Sprawdzenie, czy przycisk został wciśnięty (zbocze narastające)
        if (buttonState == GPIO_PIN_RESET && lastButtonState == GPIO_PIN_SET)
        {
            buttonPressCount++; // Zwiększ licznik kliknięć
            HAL_Delay(50); // Krótkie opóźnienie dla eliminacji drgań styków
        }

        lastButtonState = buttonState; // Aktualizacja poprzedniego stanu przycisku
    }


	return buttonPressCount;
}

void display_mode(OPERATING_MODE opermode)
{


}
