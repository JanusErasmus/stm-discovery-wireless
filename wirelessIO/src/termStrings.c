#include "termStrings.h"

char* t_strcmp(const char* str1, const char* str2)
{
	int k=0;
	 while(str1[k] == str2[k])
	 {
		 k++;

		 if(str1[k] == 0 && str2[k] == 0)
			 return 0;
		 else if (str1[k] == 0 || str2[k] == 0)
			break;
	 }

	 return (char*)&str1[k];
}

uint16_t t_strlen(const char* str)
{
	uint16_t len = 0;

	while(str[len] != 0)
	{
		if(len++ > 65000)
		{
			len = 0;
			break;
		}
	}

	return len;
}

void t_stripWhite(char * str)
{
	uint16_t len = 0;

	while(str[len] != 0)
	{
		if((str[len] == '\n') || (str[len] == '\r'))
			str[len] = 0;

		if(len++ > 65000)
		{
			len = 0;
			break;
		}

	}
}
