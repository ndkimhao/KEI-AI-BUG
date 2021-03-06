#include "stdafx.h"

std::string Format(const char* format, ...)
{
	static char str[1024 * 10];
	va_list args;

	va_start(args, format);
	vsnprintf(str, sizeof(str), format, args);
	va_end(args);

	return std::string(str);
}