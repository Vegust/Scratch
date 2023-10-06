#include "logger.h"
#include <cstdarg>
#include <cstdio>

void logger::Log(const char* Format, ...) {
	va_list ArgP;
	va_start(ArgP, Format);
	vprintf(Format, ArgP);
	fflush(stdout);
	va_end(ArgP);
}
