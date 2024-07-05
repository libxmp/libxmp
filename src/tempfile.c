/* Extended Module Player
 * Copyright (C) 1996-2024 Claudio Matsuoka and Hipolito Carraro Jr
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifdef __SUNPRO_C
#pragma error_messages (off,E_EMPTY_TRANSLATION_UNIT)
#endif

#include "common.h"

#if !(defined(LIBXMP_NO_PROWIZARD) && defined(LIBXMP_NO_DEPACKERS))

#if defined(_WIN32) || defined(__WATCOMC__)
#include <io.h>
#else
#include <unistd.h>
#endif
#ifdef HAVE_UMASK
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include "tempfile.h"

#ifdef _WIN32

#define fdopen _fdopen
#define close _close
#define unlink _unlink
#define umask _umask
int mkstemp(char *);

static char *get_temp_dir(const char *leaf)
{
	static const char def[] = "C:\\WINDOWS\\TEMP";
	const char *tmp = getenv("TEMP");
	char *path = NULL;

	if (asprintf(&path, "%s\\%s", (tmp != NULL)? tmp : def, leaf) < 0)
		return NULL;
	return path;
}

#elif defined(__OS2__) || defined(__EMX__)

static char *get_temp_dir(const char *leaf)
{
	static const char def[] = "C:";
	const char *tmp = getenv("TMP");
	char *path = NULL;

	if (asprintf(&path, "%s\\%s", (tmp != NULL)? tmp : def, leaf) < 0)
		return NULL;
	return path;
}

#elif defined(__MSDOS__) || defined(_DOS)

static char *get_temp_dir(const char *leaf)
{
	char *path = NULL;

	if (asprintf(&path, "C:\\%s", leaf) < 0)
		return NULL;
	return path;
}

#elif defined LIBXMP_AMIGA

static char *get_temp_dir(const char *leaf)
{
	char *path = NULL;

	if (asprintf(&path, "T:%s", leaf) < 0)
		return NULL;
	return path;
}

#elif defined __ANDROID__

#include <sys/types.h>
#include <sys/stat.h>

static char *get_temp_dir(const char *leaf)
{
#define APPDIR "/sdcard/Xmp for Android"
	struct stat st;
	char *path = NULL;

	if (stat(APPDIR, &st) < 0) {
		if (mkdir(APPDIR, 0777) < 0)
			return -1;
	}
	if (stat(APPDIR "/tmp", &st) < 0) {
		if (mkdir(APPDIR "/tmp", 0777) < 0)
			return -1;
	}

	if (asprintf(&path, APPDIR "/tmp/%s", leaf) < 0)
		return NULL;
	return path;
}

#else /* unix */

static char *get_temp_dir(const char *leaf)
{
	const char *tmp = getenv("TMPDIR");
	char *path = NULL;
	int ret;

	if (tmp) {
		ret = asprintf(&path, "%s/%s", tmp, leaf);
	} else {
		ret = asprintf(&path, "/tmp/%s", leaf);
	}

	return (ret < 0) ? NULL : path;
}

#endif


FILE *make_temp_file(char **filename) {
	FILE *temp;
	int fd;

	if ((*filename = get_temp_dir("xmp_XXXXXX")) == NULL)
		goto err;

#ifdef HAVE_UMASK
	umask(0177);
#endif
	if ((fd = mkstemp(*filename)) < 0)
		goto err2;

	if ((temp = fdopen(fd, "w+b")) == NULL)
		goto err3;

	return temp;

    err3:
	close(fd);
    err2:
	free(*filename);
    err:
	return NULL;
}

/*
 * Windows doesn't allow you to unlink an open file, so we changed the
 * temp file cleanup system to remove temporary files after we close it
 */
void unlink_temp_file(char *temp)
{
	if (temp) {
		unlink(temp);
		free(temp);
	}
}

#endif
