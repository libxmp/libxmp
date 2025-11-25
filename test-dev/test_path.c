#include "test.h"
#include "../src/path.h"

/* Test macros, rather than functions, to preserve caller line numbers */

#define test_move(dest, src, expected) \
do { \
	libxmp_path_move((dest), (src)); \
	if (!(expected)) { \
		fail_unless((dest)->path == NULL, "should be null"); \
	} else { \
		/* Silence nonsensical compiler warning */ \
		const char *e = (expected) ? (expected) : "stfu"; \
		fail_unless(!strcmp((dest)->path, e), (dest)->path); \
	} \
} while(0)

#define test_set(dest, value, expected) \
do { \
	ret = libxmp_path_set((dest), (value)); \
	fail_unless(ret == 0, "failed alloc"); \
	fail_unless(!strcmp((dest)->path, (expected)), (dest)->path); \
} while(0)

#define test_truncate(dest, num, expected) \
do { \
	ret = libxmp_path_truncate((dest), (num)); \
	fail_unless(ret == 0, "failed alloc"); \
	fail_unless(!strcmp((dest)->path, (expected)), (dest)->path); \
} while(0)

#define test_suffix_at(dest, ext_pos, ext, expected) \
do { \
	ret = libxmp_path_suffix_at((dest), (ext_pos), (ext)); \
	if (!(expected)) { \
		fail_unless(ret == -1, "ext pos should cause failure"); \
	} else { \
		/* Silence nonsensical compiler warning */ \
		const char *e = (expected) ? (expected) : "stfu"; \
		fail_unless(ret == 0, "failed alloc or check"); \
		fail_unless(!strcmp((dest)->path, e), (dest)->path); \
	} \
} while(0)

#define test_append(dest, value, expected) \
do { \
	ret = libxmp_path_append((dest), (value)); \
	fail_unless(ret == 0, "failed alloc or length overflowed"); \
	fail_unless(!strcmp((dest)->path, (expected)), (dest)->path); \
} while(0)

#define test_join(dest, value_a, value_b, expected) \
do { \
	ret = libxmp_path_join((dest), (value_a), (value_b)); \
	fail_unless(ret == 0, "failed alloc or length overflowed"); \
	fail_unless(!strcmp((dest)->path, (expected)), (dest)->path); \
} while(0)


TEST(test_path)
{
	struct libxmp_path sp;
	struct libxmp_path sp2;
	int ret;

	libxmp_path_init(&sp);
	/* Valid to free on init */
	libxmp_path_free(&sp);

	/* Set */
	libxmp_path_init(&sp);
	test_set(&sp, "test path", "test path");
	test_set(&sp, "another_path", "another_path");
	test_set(&sp, "i//luv//slashes//", "i/luv/slashes");
	test_set(&sp, "//////lololol///////////", "/lololol");
	test_set(&sp, "//////", "/");
	test_set(&sp, "c:\\this\\too", "c:/this/too");
	test_set(&sp, "z:\\\\lmao\\\\\\\\", "z:/lmao");

	ret = libxmp_path_set(&sp, NULL);
	fail_unless(ret == -1, "fail on NULL");

	libxmp_path_free(&sp);
	libxmp_path_free(&sp); /* Junk data but should be okay to free */

	/* Move */
	libxmp_path_init(&sp);
	libxmp_path_init(&sp2);
	test_set(&sp, "path a", "path a");
	test_set(&sp2, "path b", "path b");
	test_move(&sp, &sp2, "path b");
	libxmp_path_free(&sp);
	libxmp_path_free(&sp2); /* Junk data but should be okay to free */

	libxmp_path_init(&sp);
	libxmp_path_init(&sp2);
	test_set(&sp, "path a", "path a");
	test_move(&sp, &sp2, NULL);

	/* Truncate */
	libxmp_path_init(&sp);
	test_set(&sp, "a/test/path", "a/test/path");
	test_truncate(&sp, 10000, "a/test/path");
	test_truncate(&sp, 8, "a/test/p");
	test_truncate(&sp, 7, "a/test");
	test_truncate(&sp, 6, "a/test");
	test_truncate(&sp, 2, "a");
	test_truncate(&sp, 1, "a");
	test_truncate(&sp, 0, "");
	libxmp_path_free(&sp);

	/* Suffix at */
	libxmp_path_init(&sp);
	test_set(&sp, "another/path.s3m", "another/path.s3m");
	test_suffix_at(&sp, 12, ".mod", "another/path.mod");
	test_suffix_at(&sp, 17, ".AS", NULL);
	test_suffix_at(&sp, 16, ".nt", "another/path.mod.nt");
	test_suffix_at(&sp, 10000, ".abc", NULL);
	test_suffix_at(&sp, 16, ".AS", "another/path.mod.AS");
	test_suffix_at(&sp, 12, ".mod", "another/path.mod");
	test_suffix_at(&sp, 17, ".nt", NULL);
	test_suffix_at(&sp, 8, "\\/\\//loool.it", "another/loool.it");

	ret = libxmp_path_suffix_at(&sp, 5, NULL);
	fail_unless(ret == -1, "fail on NULL");

	libxmp_path_free(&sp);

	/* Append */
	libxmp_path_init(&sp);
	test_append(&sp, "hellow!11", "/hellow!11");
	test_truncate(&sp, 0, "");
	test_append(&sp, "/\\/\\/owo\\//\\//", "/owo");
	test_set(&sp, "init", "init");
	test_append(&sp, "more path", "init/more path");
	test_set(&sp, "first/", "first");
	test_append(&sp, "second", "first/second");
	test_append(&sp, "/third", "first/second/third");
	test_append(&sp, "", "first/second/third"); /* unintended but works */

	ret = libxmp_path_append(&sp, NULL);
	fail_unless(ret == -1, "fail on NULL");

	libxmp_path_free(&sp);

	/* Join */
	libxmp_path_init(&sp);
	test_join(&sp, "first", "second", "first/second");
	test_join(&sp, "/first/second/", "/third/fourth/",
			"/first/second/third/fourth");
	test_join(&sp, "//\\//\\//woah//", "\\//\\better//clean\\\\this\\/",
			"/woah/better/clean/this");

	test_join(&sp, "first", "", "first"); /* unintended but works */
	test_join(&sp, "", "second", "/second"); /* unintended but works */
	test_join(&sp, "", "", "/"); /* unintended but works */

	ret = libxmp_path_join(&sp, "a", NULL);
	fail_unless(ret == -1, "fail on NULL");
	ret = libxmp_path_join(&sp, NULL, "b");
	fail_unless(ret == -1, "fail on NULL");
	ret = libxmp_path_join(&sp, NULL, NULL);
	fail_unless(ret == -1, "fail on NULL");

	libxmp_path_free(&sp);
}
END_TEST
