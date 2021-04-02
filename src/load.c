/* Extended Module Player
 * Copyright (C) 1996-2018 Claudio Matsuoka and Hipolito Carraro Jr
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

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#ifdef __native_client__
#include <sys/syslimits.h>
#else
#include <limits.h>
#endif

#include "format.h"
#include "list.h"
#include "hio.h"
#include "tempfile.h"
#include "loaders/loader.h"

#ifndef LIBXMP_NO_DEPACKERS
#if !defined(HAVE_POPEN) && defined(_WIN32)
#include "win32/ptpopen.h"
#define HAVE_POPEN 1
#elif defined(__WATCOMC__)
#define popen  _popen
#define pclose _pclose
#define HAVE_POPEN 1
#endif
#endif
#ifndef LIBXMP_CORE_PLAYER
#include "md5.h"
#include "extras.h"
#endif


extern struct format_loader *format_loaders[];

void libxmp_load_prologue(struct context_data *);
void libxmp_load_epilogue(struct context_data *);
int  libxmp_prepare_scan(struct context_data *);

#ifndef LIBXMP_CORE_PLAYER
#define BUFLEN 16384
#endif

#ifndef LIBXMP_NO_DEPACKERS

#include "depacker.h"

static struct depacker *depacker_list[] = {
#if defined LIBXMP_AMIGA && !defined __AROS__
	&libxmp_depacker_xfd,
#endif
	&libxmp_depacker_zip,
	&libxmp_depacker_lha,
	&libxmp_depacker_gzip,
	&libxmp_depacker_bzip2,
	&libxmp_depacker_xz,
	&libxmp_depacker_compress,
	&libxmp_depacker_pp,
	&libxmp_depacker_sqsh,
	&libxmp_depacker_arcfs,
	&libxmp_depacker_mmcmp,
	&libxmp_depacker_muse,
	&libxmp_depacker_lzx,
	&libxmp_depacker_s404,
	&libxmp_depacker_arc,
	NULL
};

int test_oxm		(FILE *);

#ifndef HAVE_POPEN
static int execute_command(const char *cmd, const char *filename, FILE *t) {
	return -1;
}
#else
static int execute_command(const char *cmd, const char *filename, FILE *t)
{
	char line[1024], buf[BUFLEN];
	FILE *p;
	int n;

	snprintf(line, 1024, cmd, filename);

#if defined(_WIN32) || defined(__OS2__) || defined(__EMX__)
	/* Note: The _popen function returns an invalid file opaque, if
	 * used in a Windows program, that will cause the program to hang
	 * indefinitely. _popen works properly in a Console application.
	 * To create a Windows application that redirects input and output,
	 * read the section "Creating a Child Process with Redirected Input
	 * and Output" in the Win32 SDK. -- Mirko
	 */
	p = popen(line, "rb");
#else
	/* Linux popen fails with "rb" */
	p = popen(line, "r");
#endif

	if (p == NULL) {
	    return -1;
	}

	while ((n = fread(buf, 1, BUFLEN, p)) > 0) {
	    fwrite(buf, 1, n, t);
	}

	pclose (p);

	return 0;
}
#endif

static int decrunch(HIO_HANDLE **h, const char *filename, char **temp)
{
	unsigned char b[1024];
	const char *cmd;
	FILE *f, *t;
	int headersize;
	int i;
	struct depacker *depacker = NULL;

	cmd = NULL;
	*temp = NULL;
	f = (*h)->handle.file;

	headersize = fread(b, 1, 1024, f);
	if (headersize < 100) {	/* minimum valid file size */
		return 0;
	}

	/* Check built-in depackers */
	for (i = 0; depacker_list[i] != NULL; i++) {
		if (depacker_list[i]->test(b)) {
			depacker = depacker_list[i];
			D_(D_INFO "Use depacker %d", i);
			break;
		}
	}

	/* Check external commands */
	if (depacker == NULL) {
		if (b[0] == 'M' && b[1] == 'O' && b[2] == '3') {
			/* MO3 */
			D_(D_INFO "mo3");
			cmd = "unmo3 -s \"%s\" STDOUT";
		} else if (memcmp(b, "Rar", 3) == 0) {
			/* rar */
			D_(D_INFO "rar");
			cmd = "unrar p -inul -xreadme -x*.diz -x*.nfo -x*.txt "
			    "-x*.exe -x*.com \"%s\"";
		} else if (test_oxm(f) == 0) {
			/* oggmod */
			D_(D_INFO "oggmod");
			depacker = &libxmp_depacker_oxm;
		}
	}

	if (fseek(f, 0, SEEK_SET) < 0) {
		goto err;
	}

	if (depacker == NULL && cmd == NULL) {
		D_(D_INFO "Not packed");
		return 0;
	}

#if defined __ANDROID__ || defined __native_client__
	/* Don't use external helpers in android */
	if (cmd) {
		return 0;
	}
#endif

	/* When the filename is unknown (because it is a stream) don't use
	 * external helpers
	 */
	if (cmd && filename == NULL) {
		return 0;
	}

	D_(D_WARN "Depacking file... ");

	if ((t = make_temp_file(temp)) == NULL) {
		goto err;
	}

	/* Depack file */
	if (cmd) {
		D_(D_INFO "External depacker: %s", cmd);
		if (execute_command(cmd, filename, t) < 0) {
			D_(D_CRIT "failed");
			goto err2;
		}
	} else if (depacker) {
		D_(D_INFO "Internal depacker");
		if (depacker->depack(f, t) < 0) {
			D_(D_CRIT "failed");
			goto err2;
		}
	}

	D_(D_INFO "done");

	if (fseek(t, 0, SEEK_SET) < 0) {
		D_(D_CRIT "fseek error");
		goto err2;
	}

	hio_close(*h);
	*h = hio_open_file2(t);

	return (*h == NULL)? -1 : 0;

    err2:
	fclose(t);
    err:
	return -1;
}
#endif /* LIBXMP_NO_DEPACKERS */

#ifndef LIBXMP_CORE_PLAYER
static void set_md5sum(HIO_HANDLE *f, unsigned char *digest)
{
	unsigned char buf[BUFLEN];
	MD5_CTX ctx;
	int bytes_read;

	hio_seek(f, 0, SEEK_SET);

	MD5Init(&ctx);
	while ((bytes_read = hio_read(buf, 1, BUFLEN, f)) > 0) {
		MD5Update(&ctx, buf, bytes_read);
	}
	MD5Final(digest, &ctx);
}

static char *get_dirname(const char *name)
{
	char *dirname;
	const char *div;
	ptrdiff_t len;

	if ((div = strrchr(name, '/')) != NULL) {
		len = div - name + 1;
		dirname = malloc(len + 1);
		if (dirname != NULL) {
			memcpy(dirname, name, len);
			dirname[len] = 0;
		}
	} else {
		dirname = strdup("");
	}

	return dirname;
}

static char *get_basename(const char *name)
{
	const char *div;
	char *basename;

	if ((div = strrchr(name, '/')) != NULL) {
		basename = strdup(div + 1);
	} else {
		basename = strdup(name);
	}

	return basename;
}
#endif /* LIBXMP_CORE_PLAYER */

static int test_module(struct xmp_test_info *info, HIO_HANDLE *h)
{
	char buf[XMP_NAME_SIZE];
	int i;

	if (info != NULL) {
		*info->name = 0;	/* reset name prior to testing */
		*info->type = 0;	/* reset type prior to testing */
	}

	for (i = 0; format_loaders[i] != NULL; i++) {
		hio_seek(h, 0, SEEK_SET);
		if (format_loaders[i]->test(h, buf, 0) == 0) {
			int is_prowizard = 0;

#if !defined(LIBXMP_CORE_PLAYER) && !defined(LIBXMP_NO_PROWIZARD)
			if (strcmp(format_loaders[i]->name, "prowizard") == 0) {
				hio_seek(h, 0, SEEK_SET);
				pw_test_format(h, buf, 0, info);
				is_prowizard = 1;
			}
#endif

			if (info != NULL && !is_prowizard) {
				strncpy(info->name, buf, XMP_NAME_SIZE - 1);
				info->name[XMP_NAME_SIZE - 1] = '\0';

				strncpy(info->type, format_loaders[i]->name,
							XMP_NAME_SIZE - 1);
				info->type[XMP_NAME_SIZE - 1] = '\0';
			}
			return 0;
		}
	}
	return -XMP_ERROR_FORMAT;
}

int xmp_test_module(const char *path, struct xmp_test_info *info)
{
	HIO_HANDLE *h;
	struct stat st;
	int ret;
#ifndef LIBXMP_NO_DEPACKERS
	char *temp = NULL;
#endif

	if (stat(path, &st) < 0)
		return -XMP_ERROR_SYSTEM;

	if (S_ISDIR(st.st_mode)) {
		errno = EISDIR;
		return -XMP_ERROR_SYSTEM;
	}

	if ((h = hio_open(path, "rb")) == NULL)
		return -XMP_ERROR_SYSTEM;

#ifndef LIBXMP_NO_DEPACKERS
	if (decrunch(&h, path, &temp) < 0) {
		ret = -XMP_ERROR_DEPACK;
		goto err;
	}

	/* get size after decrunch */
	if (hio_size(h) < 256) {	/* set minimum valid module size */
		ret = -XMP_ERROR_FORMAT;
		goto err;
	}
#endif

	ret = test_module(info, h);

#ifndef LIBXMP_NO_DEPACKERS
    err:
	hio_close(h);
	unlink_temp_file(temp);
#else
	hio_close(h);
#endif
	return ret;
}

int xmp_test_module_from_memory(const void *mem, long size, struct xmp_test_info *info)
{
	HIO_HANDLE *h;
	int ret;

	if (size <= 0) {
		return -XMP_ERROR_INVALID;
	}

	if ((h = hio_open_mem(mem, size)) == NULL)
		return -XMP_ERROR_SYSTEM;

	ret = test_module(info, h);

	hio_close(h);
	return ret;
}

int xmp_test_module_from_file(void *file, struct xmp_test_info *info)
{
	HIO_HANDLE *h;
	int ret;
#ifndef LIBXMP_NO_DEPACKERS
	char *temp = NULL;
#endif

	if ((h = hio_open_file((FILE *)file)) == NULL)
		return -XMP_ERROR_SYSTEM;

#ifndef LIBXMP_NO_DEPACKERS
	if (decrunch(&h, NULL, &temp) < 0) {
		ret = -XMP_ERROR_DEPACK;
		goto err;
	}

	/* get size after decrunch */
	if (hio_size(h) < 256) {	/* set minimum valid module size */
		ret = -XMP_ERROR_FORMAT;
		goto err;
	}
#endif

	ret = test_module(info, h);

#ifndef LIBXMP_NO_DEPACKERS
    err:
	hio_close(h);
	unlink_temp_file(temp);
#else
	hio_close(h);
#endif
	return ret;
}

static int load_module(xmp_context opaque, HIO_HANDLE *h)
{
	struct context_data *ctx = (struct context_data *)opaque;
	struct module_data *m = &ctx->m;
	struct xmp_module *mod = &m->mod;
	int i, j, ret;
	int test_result, load_result;

	libxmp_load_prologue(ctx);

	D_(D_WARN "load");
	test_result = load_result = -1;
	for (i = 0; format_loaders[i] != NULL; i++) {
		hio_seek(h, 0, SEEK_SET);
		hio_error(h); /* reset error flag */

		D_(D_WARN "test %s", format_loaders[i]->name);
		test_result = format_loaders[i]->test(h, NULL, 0);
		if (test_result == 0) {
			hio_seek(h, 0, SEEK_SET);
			D_(D_WARN "load format: %s", format_loaders[i]->name);
			load_result = format_loaders[i]->loader(m, h, 0);
			break;
		}
	}

#ifndef LIBXMP_CORE_PLAYER
	if (test_result == 0 && load_result == 0)
		set_md5sum(h, m->md5);
#endif

	if (test_result < 0) {
		xmp_release_module(opaque);
		return -XMP_ERROR_FORMAT;
	}

	if (load_result < 0) {
		goto err_load;
	}

	/* Sanity check: number of channels, module length */
	if (mod->chn > XMP_MAX_CHANNELS || mod->len > XMP_MAX_MOD_LENGTH) {
		goto err_load;
	}

	/* Sanity check: channel pan */
	for (i = 0; i < mod->chn; i++) {
		if (mod->xxc[i].vol < 0 || mod->xxc[i].vol > 0xff) {
			goto err_load;
		}
		if (mod->xxc[i].pan < 0 || mod->xxc[i].pan > 0xff) {
			goto err_load;
		}
	}

	/* Sanity check: patterns */
	if (mod->xxp == NULL) {
		goto err_load;
	}
	for (i = 0; i < mod->pat; i++) {
		if (mod->xxp[i] == NULL) {
			goto err_load;
		}
		for (j = 0; j < mod->chn; j++) {
			int t = mod->xxp[i]->index[j];
			if (t < 0 || t >= mod->trk || mod->xxt[t] == NULL) {
				goto err_load;
			}
		}
	}

	libxmp_adjust_string(mod->name);
	for (i = 0; i < mod->ins; i++) {
		libxmp_adjust_string(mod->xxi[i].name);
	}
	for (i = 0; i < mod->smp; i++) {
		libxmp_adjust_string(mod->xxs[i].name);
	}

	libxmp_load_epilogue(ctx);

	ret = libxmp_prepare_scan(ctx);
	if (ret < 0) {
		xmp_release_module(opaque);
		return ret;
	}

	ret = libxmp_scan_sequences(ctx);
	if (ret < 0) {
		xmp_release_module(opaque);
		return -XMP_ERROR_LOAD;
	}

	ctx->state = XMP_STATE_LOADED;

	return 0;

    err_load:
	xmp_release_module(opaque);
	return -XMP_ERROR_LOAD;
}

int xmp_load_module(xmp_context opaque, const char *path)
{
	struct context_data *ctx = (struct context_data *)opaque;
#ifndef LIBXMP_CORE_PLAYER
	struct module_data *m = &ctx->m;
	long size;
#endif
#ifndef LIBXMP_NO_DEPACKERS
	char *temp_name;
#endif
	HIO_HANDLE *h;
	struct stat st;
	int ret;

	D_(D_WARN "path = %s", path);

	if (stat(path, &st) < 0) {
		return -XMP_ERROR_SYSTEM;
	}

	if (S_ISDIR(st.st_mode)) {
		errno = EISDIR;
		return -XMP_ERROR_SYSTEM;
	}

	if ((h = hio_open(path, "rb")) == NULL) {
		return -XMP_ERROR_SYSTEM;
	}

#ifndef LIBXMP_NO_DEPACKERS
	D_(D_INFO "decrunch");
	if (decrunch(&h, path, &temp_name) < 0) {
		ret = -XMP_ERROR_DEPACK;
		goto err;
	}
#endif

#ifndef LIBXMP_CORE_PLAYER
	size = hio_size(h);
	if (size < 256) {		/* get size after decrunch */
		ret = -XMP_ERROR_FORMAT;
		goto err;
	}
#endif

	if (ctx->state > XMP_STATE_UNLOADED)
		xmp_release_module(opaque);

#ifndef LIBXMP_CORE_PLAYER
	m->dirname = get_dirname(path);
	if (m->dirname == NULL) {
		ret = -XMP_ERROR_SYSTEM;
		goto err;
	}

	m->basename = get_basename(path);
	if (m->basename == NULL) {
		ret = -XMP_ERROR_SYSTEM;
		goto err;
	}

	m->filename = path;	/* For ALM, SSMT, etc */
	m->size = size;
#else
	ctx->m.filename = NULL;
	ctx->m.dirname = NULL;
	ctx->m.basename = NULL;
#endif

	ret = load_module(opaque, h);
	hio_close(h);

#ifndef LIBXMP_NO_DEPACKERS
	unlink_temp_file(temp_name);
#endif

	return ret;

#ifndef LIBXMP_CORE_PLAYER
    err:
	hio_close(h);
#ifndef LIBXMP_NO_DEPACKERS
	unlink_temp_file(temp_name);
#endif
	return ret;
#endif
}

int xmp_load_module_from_memory(xmp_context opaque, const void *mem, long size)
{
	struct context_data *ctx = (struct context_data *)opaque;
	struct module_data *m = &ctx->m;
	HIO_HANDLE *h;
	int ret;

	if (size <= 0) {
		return -XMP_ERROR_INVALID;
	}

	if ((h = hio_open_mem(mem, size)) == NULL)
		return -XMP_ERROR_SYSTEM;

	if (ctx->state > XMP_STATE_UNLOADED)
		xmp_release_module(opaque);

	m->filename = NULL;
	m->basename = NULL;
	m->dirname = NULL;
	m->size = size;

	ret = load_module(opaque, h);

	hio_close(h);

	return ret;
}

int xmp_load_module_from_file(xmp_context opaque, void *file, long size)
{
	struct context_data *ctx = (struct context_data *)opaque;
	struct module_data *m = &ctx->m;
	HIO_HANDLE *h;
	int ret;

	if ((h = hio_open_file((FILE *)file)) == NULL)
		return -XMP_ERROR_SYSTEM;

	if (ctx->state > XMP_STATE_UNLOADED)
		xmp_release_module(opaque);

	m->filename = NULL;
	m->basename = NULL;
	m->dirname = NULL;
	m->size = hio_size(h);

	ret = load_module(opaque, h);

	hio_close(h);

	return ret;
}

void xmp_release_module(xmp_context opaque)
{
	struct context_data *ctx = (struct context_data *)opaque;
	struct module_data *m = &ctx->m;
	struct xmp_module *mod = &m->mod;
	int i;

	/* can't test this here, we must call release_module to clean up
	 * load errors
	if (ctx->state < XMP_STATE_LOADED)
		return;
	 */

	if (ctx->state > XMP_STATE_LOADED)
		xmp_end_player(opaque);

	ctx->state = XMP_STATE_UNLOADED;

	D_(D_INFO "Freeing memory");

#ifndef LIBXMP_CORE_PLAYER
	libxmp_release_module_extras(ctx);
#endif

	if (mod->xxt != NULL) {
		for (i = 0; i < mod->trk; i++) {
			free(mod->xxt[i]);
		}
		free(mod->xxt);
		mod->xxt = NULL;
	}

	if (mod->xxp != NULL) {
		for (i = 0; i < mod->pat; i++) {
			free(mod->xxp[i]);
		}
		free(mod->xxp);
		mod->xxp = NULL;
	}

	if (mod->xxi != NULL) {
		for (i = 0; i < mod->ins; i++) {
			free(mod->xxi[i].sub);
			free(mod->xxi[i].extra);
		}
		free(mod->xxi);
		mod->xxi = NULL;
	}

	if (mod->xxs != NULL) {
		for (i = 0; i < mod->smp; i++) {
			libxmp_free_sample(&mod->xxs[i]);
		}
		free(mod->xxs);
		mod->xxs = NULL;
	}

	free(m->xtra);
	m->xtra = NULL;

#ifndef LIBXMP_CORE_DISABLE_IT
	if (m->xsmp != NULL) {
		for (i = 0; i < mod->smp; i++) {
			libxmp_free_sample(&m->xsmp[i]);
		}
		free(m->xsmp);
		m->xsmp = NULL;
	}
#endif

	libxmp_free_scan(ctx);

	free(m->comment);
	m->comment = NULL;

	D_("free dirname/basename");
	free(m->dirname);
	free(m->basename);
	m->basename = NULL;
	m->dirname = NULL;
}

void xmp_scan_module(xmp_context opaque)
{
	struct context_data *ctx = (struct context_data *)opaque;

	if (ctx->state < XMP_STATE_LOADED)
		return;

	libxmp_scan_sequences(ctx);
}
