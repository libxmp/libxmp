/* _[v]snprintf() from msvcrt.dll might not nul terminate */
/* OpenWatcom-provided versions seem to behave the same... */

#include "common.h"

#if defined(USE_LIBXMP_ASPRINTF)

#undef asprintf

int libxmp_asprintf(char **strp, const char *fmt, ...)
{
	int n;
	int size = 100;     /* Guess we need no more than 100 bytes */
	char *p, *np;
	va_list ap;

	*strp = NULL;

	if ((p = malloc(size)) == NULL)
		return -1;

	while (1) {
		/* Try to print in the allocated space */
		va_start(ap, fmt);
		n = vsnprintf(p, size, fmt, ap);
		va_end(ap);

		/* Check error code */
		if (n < 0) {
			free(p);
			return -1;
		}

		/* If that worked, return the string */
		if (n < size) {
			*strp = p;
			return n;
		}

		/* Else try again with more space */
		size = n + 1; /* Precisely what is needed */

		if ((np = realloc (p, size)) == NULL) {
			free(p);
			return -1;
		} else {
			p = np;
		}
	}
}

#endif

#if defined(USE_LIBXMP_SNPRINTF)

#undef snprintf
#undef vsnprintf

int libxmp_vsnprintf(char *str, size_t sz, const char *fmt, va_list ap)
{
	int rc = _vsnprintf(str, sz, fmt, ap);
	if (sz != 0) {
		if (rc < 0) rc = (int)sz;
		if ((size_t)rc >= sz) str[sz - 1] = '\0';
	}
	return rc;
}

int libxmp_snprintf (char *str, size_t sz, const char *fmt, ...)
{
	va_list ap;
	int rc;

	va_start (ap, fmt);
	rc = _vsnprintf(str, sz, fmt, ap);
	va_end (ap);

	return rc;
}

#endif

/* Win32 debug message helper by Mirko Buffoni */
#if defined(_MSC_VER) && defined(DEBUG)
void libxmp_msvc_dbgprint(const char *format, ...)
{
	va_list argptr;

	/* do the output */
	va_start(argptr, format);
	vprintf(format, argptr);
	printf("\n");
	va_end(argptr);
}
#endif
