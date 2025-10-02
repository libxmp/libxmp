/* Extended Module Player
 * Copyright (C) 1996-2025 Claudio Matsuoka and Hipolito Carraro Jr
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

#ifndef LIBXMP_PATH_H
#define LIBXMP_PATH_H

#include "common.h"

struct libxmp_path
{
	char *path;
	size_t length;
	size_t alloc;
};

LIBXMP_BEGIN_DECLS

void	libxmp_path_init	(struct libxmp_path *);
void	libxmp_path_free	(struct libxmp_path *);
void	libxmp_path_move	(struct libxmp_path *dest, struct libxmp_path *src);
int	libxmp_path_set		(struct libxmp_path *, const char *);
int	libxmp_path_truncate	(struct libxmp_path *, size_t);
int	libxmp_path_suffix_at	(struct libxmp_path *, size_t, const char *);
int	libxmp_path_append	(struct libxmp_path *, const char *);
int	libxmp_path_join	(struct libxmp_path *, const char *, const char *);

LIBXMP_END_DECLS

#endif /* LIBXMP_PATH_H */
