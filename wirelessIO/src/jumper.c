#include <stm32f10x.h>
#include <misc.h>

#include "jumper.h"
#include "terminal.h"

typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;

void jump(uint32_t address)
{
	t_print("Going HSI clock\n\n");

	//switch on HSI
	RCC->CR |= (uint32_t)0x00000001;
	while ((RCC->CR & (uint32_t)RCC_CR_HSIRDY) != (uint32_t)RCC_CR_HSIRDY)
	{
		t_print("wait HSI\n");
	}

	/* Select HSI as system clock source */
	RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
	RCC->CFGR |= (uint32_t)RCC_CFGR_SW_HSI;

	/* Wait till HSI is used as system clock source */
	while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x00)
	{
	}

	/* Reset RCC */
	RCC->CR = (uint32_t)0x00000081;

	/* Reset CFGR */
	RCC->CFGR = (uint32_t)0x00000000;

	/* Reset CFGR2 register */
	RCC->CFGR2 = (uint32_t)0x00000000;

	/* Disable all interrupts and clear pending bits  */
	RCC->CIR = 0x00FF0000;
	FLASH->ACR = 0x03;


	JumpAddress = *( uint32_t*) (address + 4);
	Jump_To_Application = (pFunction) JumpAddress;

	/* Initialize user application's Stack Pointer */
	__set_MSP(*(uint32_t*) address);
	Jump_To_Application();
}
