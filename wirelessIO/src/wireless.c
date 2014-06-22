#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>
#include <misc.h>

#include "terminal.h"
#include "wireless.h"
#include "crc.h"
#include "temp.h"

#define WBUFF_LEN 64

uint8_t wRXbuff[WBUFF_LEN];
uint32_t wLen = 0;

uint8_t wTXbuff[WBUFF_LEN];

uint8_t frameHandled = 1;
uint8_t wFbuff[WBUFF_LEN];
uint32_t fLen = 0;
uint8_t validFrame = 0;
uint8_t await_new_frame = 1;
uint8_t escaping = 0;

void delayByteTx(void);

void initWireless(void)
{
	//enable USART 3 clock
	RCC_APB1PeriphClockCmd(RCC_APB1ENR_USART3EN, ENABLE);

	//Enable GPIOB clock
	RCC_APB2PeriphClockCmd(RCC_APB2ENR_IOPBEN, ENABLE);

	//Remap to PC10 and PC11
	//GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);

	GPIO_InitTypeDef txPin;

	txPin.GPIO_Pin = GPIO_Pin_10;
	txPin.GPIO_Mode = GPIO_Mode_AF_PP;
	txPin.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &txPin);

	GPIO_InitTypeDef rxPin;
	rxPin.GPIO_Pin = GPIO_Pin_11;
	rxPin.GPIO_Mode = GPIO_Mode_IPU;
	rxPin.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &rxPin);

	USART_Cmd(USART3, ENABLE);

	USART_InitTypeDef usartSetup;
	usartSetup.USART_BaudRate = 2400;
	usartSetup.USART_WordLength = USART_WordLength_8b;
	usartSetup.USART_StopBits = USART_StopBits_1;
	usartSetup.USART_Parity = USART_Parity_No;
	usartSetup.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	usartSetup.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART3, &usartSetup);

	//Map UART interrupt
	NVIC_InitTypeDef rxInt;
	rxInt.NVIC_IRQChannel = USART3_IRQn;
	rxInt.NVIC_IRQChannelCmd = ENABLE;
	rxInt.NVIC_IRQChannelPreemptionPriority = 3;
	rxInt.NVIC_IRQChannelSubPriority = 4;
	NVIC_Init(&rxInt);

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
}

void w_send(uint8_t * buff, uint8_t len)
{
	GPIO_InitTypeDef pin;
	pin.GPIO_Pin = GPIO_Pin_10;
	pin.GPIO_Mode = GPIO_Mode_AF_PP;
	pin.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &pin);

	uint32_t txLen = WBUFF_LEN;
	HDLC_Frame(buff, len, wTXbuff, &txLen);

	for(int l =0; l < 5; l++)
	{
		for(int k = 0; k < txLen; k++)
		{

			USART_SendData(USART3, wTXbuff[k]);
			while(!USART_GetFlagStatus(USART3, USART_FLAG_TXE));
		}
	}
	delayByteTx();

	pin.GPIO_Pin = GPIO_Pin_10;
	pin.GPIO_Mode = GPIO_Mode_Out_PP;
	pin.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &pin);

	GPIO_ResetBits(GPIOB, GPIO_Pin_10);
}

void delayByteTx()
{
	volatile int t = 15000;

	while(t > 0 )
		t--;
}

void USART3_IRQHandler(void)
{
	uint8_t data =	USART3->DR;


	if(wLen++ > WBUFF_LEN)
		wLen = 0;

	//extarct HDLC frame as bytes arive
	{
		//diag_printf( " 0x%02X", buff[i] );
		switch (data)
		{
		// Receive a flag
		case 0x7E:
			// Is this the start of a new frame?
			if (await_new_frame)
			{
				// New frame has started
				await_new_frame = 0;

				fLen = 0;
			}
			// Or was it the end of the current frame?
			else
			{
				// Current frame has ended
				await_new_frame = 1;
				// Process the received command
				validFrame = 1;
			} // else
			break;

			// Receive a escape character
		case 0x7D:
			// Next byte will be escaped
			escaping = 1;
			break;

		default:
			// Should byte be escaped?
			if (escaping)	// Yes
			{
				// XOR with 0x20 to get original byte
				wFbuff[fLen] = data ^ 0x20;
				if(fLen++ > WBUFF_LEN) //check for repeated incorrect data
				{
					await_new_frame = 1; //end frame receive
					fLen =  0;
				}

				// Done escaping
				escaping = 0;
			}
			else
			{
				// Normal byte
				wFbuff[fLen] = data;
				if(fLen++ > WBUFF_LEN) //check for repeated incorrect data
				{
					await_new_frame = 1; //end frame receive
					fLen =  0;
				}
			}
			break;
		}

		if(validFrame && fLen && fLen < WBUFF_LEN)
		{
			if(ccitt_crc16(wFbuff, fLen) == GOOD_CRC)
			{
				wLen = fLen - 2;
				frameHandled = 0;
				for(int k = 0; k < wLen; k++)
				{
					wRXbuff[k] = wFbuff[k];
				}
				uint16_t val = wRXbuff[0] | (wRXbuff[1] << 8);
				t_print(getTempLookupString(val));

				t_print("\r\n");
				validFrame = 0;
				fLen = 0;
			}
		}
	}
}

uint32_t w_read(uint8_t* buff, uint32_t len)
{
	if(frameHandled)
		return 0;

	if(len > wLen)
		len = wLen;

	for(uint32_t k = 0; k < len; k++)
	{
		buff[k] = wRXbuff[k];
	}

	wLen = 0;
	frameHandled = 1;

	return len;
}
