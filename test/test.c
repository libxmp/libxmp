#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/md5.h"
#include "xmp.h"

#ifndef LIBXMP_CORE_DISABLE_IT

#define TEST_IT_FILE		"test.it"
#ifndef LIBXMP_NO_DEPACKERS
#define TEST_ITZ_FILE		"test.itz"
#endif
#define TEST_IT_PLAYTIME	4800
#define TEST_IT_MD5_IEEE	"3464d82e06b7f71763cf52abb37c3b36"
#define TEST_IT_MD5_X87 	"2d5cfea801bc2367f8a397a0803b5823"

#endif

#define TEST_XM_FILE		"test.xm"
#define TEST_XM_PLAYTIME	8000
#define TEST_XM_MD5_IEEE	"7cece79ea1d96b6e35bccceecf73883a"
#define TEST_XM_MD5_X87 	"7cece79ea1d96b6e35bccceecf73883a"

static inline int is_big_endian(void) {
	unsigned short w = 0x00ff;
	return (*(char *)&w == 0x00);
}

/* Convert little-endian 16 bit samples to big-endian */
static void convert_endian(unsigned char *p, int l)
{
	unsigned char b;
	int i;

	for (i = 0; i < l; i++) {
		b = p[0];
		p[0] = p[1];
		p[1] = b;
		p += 2;
	}
}

static void print_md5(const unsigned char *d)
{
	int i;

	printf("\n");
	for (i = 0; i < 16 ; i++) {
		printf("%02x", d[i]);
	}
	printf(" ");
}

static int compare_md5(const unsigned char *d, const char *digest)
{
	int i;

	for (i = 0; i < 16 && *digest; i++, digest += 2) {
		char hex[3];
		hex[0] = digest[0];
		hex[1] = digest[1];
		hex[2] = 0;

		if (d[i] != strtoul(hex, NULL, 16))
			return -1;
	}

	return 0;
}

static int run_test(const char *file, const char *md5_ieee, const char *md5_x87, int playtime)
{
	int ret;
	xmp_context c;
	struct xmp_frame_info info;
	long time;
	unsigned char digest[16];
	MD5_CTX ctx;

	c = xmp_create_context();
	if (c == NULL) {
		printf("failed creating context\n");
		goto err;
	}

	ret = xmp_load_module(c, file);
	if (ret != 0) {
		printf("can't load module\n");
		goto err;
	}

	xmp_get_frame_info(c, &info);
	if (info.total_time != playtime) {
		printf("estimated replay time error: %d\n", info.total_time);
		goto err;
	}

	xmp_start_player(c, 22050, 0);
	xmp_set_player(c, XMP_PLAYER_MIX, 100);
	xmp_set_player(c, XMP_PLAYER_INTERP, XMP_INTERP_SPLINE);

	printf("Testing ");
	fflush(stdout);
	time = 0;

	MD5Init(&ctx);

	while (1) {
		xmp_play_frame(c);
		xmp_get_frame_info(c, &info);
		if (info.loop_count > 0)
			break;

		time += info.frame_time;

		if (is_big_endian())
			convert_endian((unsigned char *)info.buffer, info.buffer_size >> 1);

		MD5Update(&ctx, (unsigned char *)info.buffer, info.buffer_size);

		printf(".");
		fflush(stdout);
	}

	MD5Final(digest, &ctx);
	print_md5(digest);

	/*
	  x87 floating point results in a very slightly different output from
	  SSE and other floating point implementations, so check two hashes.
	 */
	if (compare_md5(digest, md5_ieee) < 0 &&
	    compare_md5(digest, md5_x87) < 0) {
		printf("rendering error\n");
		goto err;
	}

	if ((time + 500) / 1000 != info.total_time) {
		printf("replay time error: %ld\n", time);
		goto err;
	}

	printf(" pass\n");

	xmp_release_module(c);
	xmp_free_context(c);
	return 0;

    err:
	printf(" fail\n");
	if (c) {
		xmp_release_module(c);
		xmp_free_context(c);
	}
	return -1;
}

int main(void)
{
	int errors = 0;

#ifdef TEST_IT_FILE
	printf("TESTING %s:\n", TEST_IT_FILE);
	errors += run_test(TEST_IT_FILE, TEST_IT_MD5_IEEE, TEST_IT_MD5_X87, TEST_IT_PLAYTIME);
#endif
#ifdef TEST_ITZ_FILE
	printf("TESTING %s:\n", TEST_ITZ_FILE);
	errors += run_test(TEST_ITZ_FILE, TEST_IT_MD5_IEEE, TEST_IT_MD5_X87, TEST_IT_PLAYTIME);
#endif
#ifdef TEST_XM_FILE
	printf("TESTING %s:\n", TEST_XM_FILE);
	errors += run_test(TEST_XM_FILE, TEST_XM_MD5_IEEE, TEST_XM_MD5_X87, TEST_XM_PLAYTIME);
#endif

	return errors;
}
