#include <stm32f10x.h>

#include "terminal.h"
#include "termHandler.h"
#include "termStrings.h"
#include "version.h"

typedef void (*func_t)();

void help();
void version();
void reset();

typedef struct
{
	char* cmd;
	char* desc;
	func_t f;

}termCommands;

termCommands cmdTable[] = {
		{"h", "Shows this help", help},
		{"ver", "Bootloader Version", version},
		{"reset", "Reset Processor", reset},
		{"jump", "Jump to loaded firmware", t_setJumpFlag},
		{0,0,0}
};

bool t_Handler(char* cmd)
{
	bool stat = 0;

	uint8_t k = 0;
	while(cmdTable[k].f)
	{
		t_stripWhite(cmd);

		if(!t_strcmp(cmdTable[k].cmd, cmd))
		{
			stat = 1;
			cmdTable[k].f();
		}


		k++;
	}

	return stat;
}

void version()
{
	t_print("Boot loader Version:");
	h_print(VERSION_NUMBER);
	t_print("Build Date: ");
	t_print(__DATE__);
	t_print(" ");
	t_print(__TIME__);
	t_print("\n");
}


typedef struct
{
  __IO uint32_t resetRegister;
}SPECIAL_RESET_REG;
void reset()
{
	t_print("Processor will RESET\n");

	uint32_t resetReg = 0xE000ED00 + 0x0C;
	((SPECIAL_RESET_REG *) resetReg)->resetRegister = (0x5FA << 16) | (1 << 2);
}


void help(const char* argv)
{
	t_print("Help:\n");
	uint8_t k = 0;
	while(cmdTable[k].f)
	{
		t_print(" ");
		t_print(cmdTable[k].cmd);
		t_print(" - ");
		t_print(cmdTable[k].desc);
		t_print("\n");

		k++;
	}
}
