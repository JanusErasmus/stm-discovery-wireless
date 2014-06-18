#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>
#include <misc.h>

#include "wireless.h"


void delayByteTx(void);

void initWireless(void)
{
	//enable USART 2 clock
	RCC_APB1PeriphClockCmd(RCC_APB1ENR_USART2EN, ENABLE);

	//Enable GPIOA clock
	RCC_APB2PeriphClockCmd(RCC_APB2ENR_IOPAEN, ENABLE);


	GPIO_InitTypeDef pin;
	pin.GPIO_Pin = GPIO_Pin_2;
	pin.GPIO_Mode = GPIO_Mode_AF_PP;
	pin.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &pin);

	pin.GPIO_Pin = GPIO_Pin_3;
	pin.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	pin.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &pin);

	USART_Cmd(USART2, ENABLE);

	USART_InitTypeDef usartSetup;
	usartSetup.USART_BaudRate = 2400;
	usartSetup.USART_WordLength = USART_WordLength_8b;
	usartSetup.USART_StopBits = USART_StopBits_1;
	usartSetup.USART_Parity = USART_Parity_No;
	usartSetup.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	usartSetup.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART2, &usartSetup);

}

void w_send(uint8_t * buff, uint8_t len)
{
	GPIO_InitTypeDef pin;
	pin.GPIO_Pin = GPIO_Pin_2;
	pin.GPIO_Mode = GPIO_Mode_AF_PP;
	pin.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &pin);

	for(int k = 0; k < len; k++)
	{
		USART_SendData(USART2, buff[k]);
		while(!USART_GetFlagStatus(USART2, USART_FLAG_TXE));
	}

	delayByteTx();

	pin.GPIO_Pin = GPIO_Pin_2;
	pin.GPIO_Mode = GPIO_Mode_Out_PP;
	pin.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &pin);

	GPIO_ResetBits(GPIOA, GPIO_Pin_2);
}

void delayByteTx()
{
	volatile int t = 15000;

	while(t >0 )
	{
		t--;
		//GPIO_SetBits(GPIOD, GPIO_Pin_4);
		t--;
		//GPIO_ResetBits(GPIOD, GPIO_Pin_4);
	}
}
