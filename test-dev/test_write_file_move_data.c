#include "test.h"

TEST(test_write_file_move_data)
{
	FILE *f1, *f2;
	uint8 b1[4000], b2[4000];
	long size;
	int ret;

	f1 = fopen("data/bzip2data", "rb");
	fail_unless(f1 != NULL, "can't open source file");
	f2 = fopen("write_test", "wb");
	fail_unless(f1 != NULL, "can't open destination file");

	move_data(f2, f1, 4000);

	fclose(f1);
	fclose(f2);

	f1 = fopen("data/bzip2data", "rb");
	f2 = fopen("write_test", "rb");

	ret = fread(b1, 1, 4000, f1);
	fail_unless(ret == 4000, "read error (f1)");
	ret = fread(b2, 1, 4000, f2);
	fail_unless(ret == 4000, "read error (f2)");
	fseek(f2, 0, SEEK_END);
	size = ftell(f2);
	fclose(f1);
	fclose(f2);

	fail_unless(size == 4000, "wrong size");
	fail_unless(memcmp(b1, b2, 4000) == 0, "read error");
	unlink("write_test");
}
END_TEST
