#include "led.h"

#define GREEN_PIN GPIO_Pin_9
#define BLUE_PIN GPIO_Pin_8

void initLED()
{
	//Enable GPIOC clock
	RCC_APB2PeriphClockCmd(RCC_APB2ENR_IOPCEN, ENABLE);

	GPIO_InitTypeDef pin;
	pin.GPIO_Mode = GPIO_Mode_Out_PP;
	pin.GPIO_Speed = GPIO_Speed_2MHz;

	pin.GPIO_Pin = GREEN_PIN;
	GPIO_Init(GPIOC, &pin);
	GPIO_ResetBits(GPIOC, GREEN_PIN);

	pin.GPIO_Pin = BLUE_PIN;
	GPIO_Init(GPIOC, &pin);
	GPIO_ResetBits(GPIOC, BLUE_PIN);
}

void led_toggleBlue()
{
	uint8_t stat = GPIO_ReadInputDataBit(GPIOC, BLUE_PIN);

	if(stat)
		GPIO_ResetBits(GPIOC, BLUE_PIN);
	else
		GPIO_SetBits(GPIOC, BLUE_PIN);
}

void led_toggleGreen()
{
	uint8_t stat = GPIO_ReadInputDataBit(GPIOC, GREEN_PIN);

		if(stat)
			GPIO_ResetBits(GPIOC, GREEN_PIN);
		else
			GPIO_SetBits(GPIOC, GREEN_PIN);
}
