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

#include "path.h"

void libxmp_path_init(struct libxmp_path *p)
{
	memset(p, 0, sizeof(struct libxmp_path));
}

void libxmp_path_free(struct libxmp_path *p)
{
	free(p->path);
	p->path = NULL;
}

void libxmp_path_move(struct libxmp_path *dest, struct libxmp_path *src)
{
	libxmp_path_free(dest);
	*dest = *src;
	src->path = NULL;
}

static int fix_size(struct libxmp_path *p, size_t sz)
{
	if (!p->path || p->alloc < sz) {
		void *tmp = realloc(p->path, sz);
		if (!tmp) {
			return -1;
		}
		p->path = (char *)tmp;
		p->alloc = sz;
	}
	return 0;
}

static int is_slash(char c)
{
	return c == '\\' || c == '/';
}

/* Some console SDKs (3DS, possibly others) handle duplicate and trailing
 * slashes very poorly. This function should be used to guarantee any path
 * created by these functions will not have these issues. */
static void clean_slashes(struct libxmp_path *p, size_t pos)
{
	size_t i, j;
	if (pos >= p->length)
		return;

	/* Normalize to /, detect duplicates */
	for (i = pos; i < p->length; i++) {
		if (is_slash(p->path[i])) {
			p->path[i] = '/';
			if (i + 1 < p->length && is_slash(p->path[i + 1])) {
				break;
			}
		}
	}
	/* Clean duplicates if they exist */
	for (j = i; i < p->length; ) {
		if (is_slash(p->path[i])) {
			p->path[j++] = '/';
			i++;
			while (i < p->length && is_slash(p->path[i])) {
				i++;
			}
		} else {
			p->path[j++] = p->path[i++];
		}
	}
	/* Trim trailing slash, except at index 0 */
	if (j > 1 && is_slash(p->path[j - 1])) {
		j--;
	}
	p->path[j] = '\0';
	p->length = j;
}

int libxmp_path_set(struct libxmp_path *p, const char *new_path)
{
	size_t sz;

	if (new_path == NULL) {
		return -1;
	}
	sz = strlen(new_path) + 1u;

	if (fix_size(p, sz) < 0) {
		return -1;
	}
	memcpy(p->path, new_path, sz);
	p->length = sz - 1u;

	clean_slashes(p, 0);
	return 0;
}

int libxmp_path_truncate(struct libxmp_path *p, size_t new_sz)
{
	if (new_sz > p->length) {
		return 0;
	}
	if (fix_size(p, new_sz + 1u) < 0) {
		return -1;
	}
	p->path[new_sz] = '\0';
	p->length = new_sz;
	if (new_sz) {
		clean_slashes(p, new_sz - 1u);
	}
	return 0;
}

int libxmp_path_suffix_at(struct libxmp_path *p, size_t ext_pos, const char *ext)
{
	size_t sz;

	if (ext == NULL || ext_pos > p->length) {
		return -1;
	}
	sz = strlen(ext) + 1u;

	if (fix_size(p, ext_pos + sz) < 0) {
		return -1;
	}
	memcpy(p->path + ext_pos, ext, sz);
	p->length = ext_pos + sz - 1u;

	clean_slashes(p, ext_pos ? ext_pos - 1u : 0);
	return 0;
}

int libxmp_path_append(struct libxmp_path *p, const char *append_path)
{
	size_t append_sz;
	size_t new_sz;
	size_t old_sz;

	if (append_path == NULL) {
		return -1;
	}
	append_sz = strlen(append_path) + 1;
	new_sz = p->length + 1u + append_sz;
	old_sz = p->length;

	if (new_sz < p->length || fix_size(p, new_sz) < 0) {
		return -1;
	}
	p->path[p->length++] = '/';
	memcpy(p->path + p->length, append_path, append_sz);
	p->length = new_sz - 1u;

	clean_slashes(p, old_sz ? old_sz - 1u : 0);
	return 0;
}

int libxmp_path_join(struct libxmp_path *p, const char *prefix_path,
	const char *suffix_path)
{
	int ret;

	if (prefix_path == NULL || suffix_path == NULL) {
		return -1;
	}

	ret = snprintf(NULL, 0, "%s/%s", prefix_path, suffix_path);
	if (ret < 0 || fix_size(p, (size_t)ret + 1u) < 0) {
		return -1;
	}
	p->length = snprintf(p->path, p->alloc, "%s/%s", prefix_path, suffix_path);

	clean_slashes(p, 0);
	return 0;
}
