
#include "stm32f10x.h"
#include "misc.h"
#include "stm32f10x_flash.h"

#include "terminal.h"
#include "jumper.h"
#include "wireless.h"
#include "led.h"

void delay();

typedef int size_t;
void* memcpy( void * destination, const void * source, size_t num );

/// Main function.  Called by the startup code.
int main(void)
{
	//when interrupts are going to be used
	NVIC_Configuration();

	initTerminal();
	initWireless();
	initLED();

	t_print("Wireless IO\n");
	t_print(__DATE__);
	t_print(" ");
	t_print(__TIME__);
	t_print("\n");

	uint8_t buff[] = {"Hello daar dis, 'n baie lang sin\r\nEn nog so bietjie\r\n"};

	while(!t_jumpNow())
	{
		w_send(buff,t_strlen((char*)buff));
		delay();
		//t_print(".");
	}

	t_print("Jumping\n");
	jump(0x08010000);

	return 0;
}

void* memcpy( void * destination, const void * source, int num )
{
	uint8_t* d = destination;
	const uint8_t* s = source;

	for(int k= 0; k < num; k++)
		d[k] = s[k];

	return 0;
}

void delay()
{
	volatile int t = 15000000;

	while(t >0 )
	{
		t--;
		//GPIO_SetBits(GPIOD, GPIO_Pin_4);
		t--;
		//GPIO_ResetBits(GPIOD, GPIO_Pin_4);
	}
}

