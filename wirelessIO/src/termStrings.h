#ifndef TERMSTRINGS_H_
#define TERMSTRINGS_H_
#include <stdint.h>

uint16_t t_strlen(const char* str);
char* t_strcmp(const char* str1, const char* str2);
void t_stripWhite(char * str);

#endif /* TERMSTRINGS_H_ */
