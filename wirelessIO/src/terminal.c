#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>
#include <misc.h>

#include "terminal.h"
#include "termHandler.h"
#include "jumper.h"

#define TERM_BUFF_LEN 64
#define TERM_ECHO
#define TERM_MAP_CR_CRNL
//#define TERM_MAP_NL_CRNL

void handleTerm();

bool t_InputFlag = 0;
bool t_jumpFlag = 0;

char t_buff[TERM_BUFF_LEN];
uint8_t t_buffLen = 0;

void initTerminal(void)
{
	//enable USART 1 clock
	RCC_APB2PeriphClockCmd(RCC_APB2ENR_USART1EN, ENABLE);

	//enable Alternate function reg
	RCC_APB2PeriphClockCmd(RCC_APB2ENR_AFIOEN, ENABLE);

	//Enable GPIOA clock
	RCC_APB2PeriphClockCmd(RCC_APB2ENR_IOPAEN, ENABLE);


	GPIO_InitTypeDef pin;
	pin.GPIO_Pin = GPIO_Pin_9;
	pin.GPIO_Mode = GPIO_Mode_AF_PP;
	pin.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &pin);

	pin.GPIO_Pin = GPIO_Pin_10;
	pin.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	pin.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &pin);

	USART_Cmd(USART1, ENABLE);

	USART_InitTypeDef usartSetup;
	usartSetup.USART_BaudRate = 115200;
	usartSetup.USART_WordLength = USART_WordLength_8b;
	usartSetup.USART_StopBits = USART_StopBits_1;
	usartSetup.USART_Parity = USART_Parity_No;
	usartSetup.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	usartSetup.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART1, &usartSetup);


	//Map UART interrupt
	NVIC_InitTypeDef termInt;
	termInt.NVIC_IRQChannel = USART1_IRQn;
	termInt.NVIC_IRQChannelCmd =ENABLE;
	termInt.NVIC_IRQChannelPreemptionPriority = 1;
	termInt.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&termInt);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);


}

bool t_hadInput()
{
	return t_InputFlag;
}


bool t_jumpNow()
{
	return t_jumpFlag;
}

void t_setJumpFlag()
{
	t_jumpFlag = 1;
}


void handleTerm()
{
	t_buff[t_buffLen] = 0;

	if(!t_Handler(t_buff) && t_buffLen > 2)
	{
		t_print("Unknown command \"");
		t_print(t_buff);
		t_print("\". Type h for list of commands\n");
	}

		t_print(">>");

	t_buffLen = 0;
}

void t_print(const char* str)
{
	uint16_t len = t_strlen(str);

	for(int k = 0; k < len; k++)
	{
		if(str[k] == '\n')
			t_putc('\r');

		t_putc(str[k]);
	}
}

void t_putc(char c)
{
	USART_SendData(USART1, c);
	while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
}





void showReg(const char* name, uint32_t reg)
{
	t_print(name);
	t_print(": ");
	h_print(reg);
}

void h_print(const uint32_t reg)
{
	t_print("0x");
	uint8_t digit = (reg >> 28) & 0x0F;
	d_print(digit);


	digit = (reg >> 24) & 0x0F;
	d_print(digit);

	digit = (reg >> 20) & 0x0F;
	d_print(digit);

	digit = (reg >> 16) & 0x0F;
	d_print(digit);

	digit = (reg >> 12) & 0x0F;
	d_print(digit);

	digit = (reg >> 8) & 0x0F;
	d_print(digit);

	digit = (reg >> 4) & 0x0F;
	d_print(digit);

	digit = (reg >> 0) & 0x0F;
	d_print(digit);

	t_print("\n");
}

void d_print(const uint8_t reg)
{
	uint8_t digit = reg + 0x30;
	if(digit > 0x39)
	{
		digit = (reg - 10) + 0x41;
	}

	USART_SendData(USART1, digit);
	while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
}

void USART1_IRQHandler(void)
{
	t_InputFlag = 1;

	uint32_t data =	USART1->DR;

#ifdef TERM_ECHO
	t_putc(data);
#endif

#ifdef TERM_MAP_CR_CRNL
	if(data == '\r')
	{
		t_buff[t_buffLen++] = data;
		t_buff[t_buffLen++] = '\n';
		t_putc('\n');
	}else
#elif TERM_MAP_NL_CRNL
	if(data == '\n')
	{
		t_buff[t_buffLen++] = '\r';
		t_buff[t_buffLen++] = data;
		t_putc('\r');
	}else
#endif
		t_buff[t_buffLen++] = data;

	if(t_buff[t_buffLen - 1] == '\n')
	{
		handleTerm();
	}


	if(t_buffLen > TERM_BUFF_LEN)
		handleTerm();

}


