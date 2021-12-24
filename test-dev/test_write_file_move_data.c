#include "test.h"
#include "../src/hio.h"

TEST(test_write_file_move_data)
{
	FILE *out;
	HIO_HANDLE *h1,*h2;
	uint8 b1[4000], b2[4000];
	long size;
	int ret;

	h1 = hio_open("data/bzip2data", "rb");
	fail_unless(h1 != NULL, "can't open source file");
	out = fopen("write_test", "wb");
	fail_unless(h1 != NULL, "can't open destination file");

	hio_move_data(out, h1, 4000);

	hio_close(h1);
	fclose(out);

	h1 = hio_open("data/bzip2data", "rb");
	h2 = hio_open("write_test", "rb");

	ret = hio_read(b1, 1, 4000, h1);
	fail_unless(ret == 4000, "read error (h1)");
	ret = hio_read(b2, 1, 4000, h2);
	fail_unless(ret == 4000, "read error (h2)");
	hio_seek(h2, 0, SEEK_END);
	size = hio_tell(h2);
	hio_close(h1);
	hio_close(h2);

	fail_unless(size == 4000, "wrong size");
	fail_unless(memcmp(b1, b2, 4000) == 0, "read error");
	unlink("write_test");
}
END_TEST
