#ifndef STRINGS_H
#define STRINGS_H

#include <stdint.h>
#include <stdbool.h>

uint8_t		StrLength(uint8_t *sa);
bool	StrComp(uint8_t *sa, uint8_t *sb);
void	StrCopy(uint8_t *sa, uint8_t *sb, uint8_t max);
uint8_t 		StrCopyIn(uint8_t *sa, uint8_t *sb, uint8_t pos, uint8_t max);
void	StrClear(uint8_t *s, uint8_t length);
int32_t 	StrToInt(uint8_t *string);
float 	StrToFloat(uint8_t *string);
double	StrToDouble(uint8_t *string);
void IntToStr(int32_t data, uint8_t *str, uint8_t sign, uint8_t digits);
void IntToStrZero(int32_t data, uint8_t *str, uint8_t sign, uint8_t digits);
void	DoubleToStr(double value, uint8_t *string, uint8_t intdigits, uint8_t digits);
void 	FloatToStr(float value, uint8_t *string, uint8_t intdigits, uint8_t digits);
void	CharToHex(uint8_t data, uint8_t *str);
uint8_t		HexToChar(uint8_t *str);
void 	UpCase(uint8_t *s);
void 	DownCase(uint8_t *s);

#endif
