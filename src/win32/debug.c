/*
 * Win32 debug message helper by Mirko Buffoni
 */
#if defined(_MSC_VER) && defined(DEBUG)

#include "common.h"

void D_(const char *format, ...)
{
	va_list argptr;

	/* do the output */
	va_start(argptr, format);
	vprintf(format, argptr);
	printf("\n");
	va_end(argptr);
}
#endif
