/**
 * @file
 * @brief Creates /dev/null
 *
 * @date 08.09.11
 * @author Anton Kozlov -- original implementation
 * @author Denis Deryugin <deryugin.denis@gmail.com> -- port to new vfs
 */
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/uio.h>

#include <util/err.h>

#include <drivers/char_dev.h>

#define NULL_DEV_NAME "null"

static void null_close(struct idesc *desc) {
}

static ssize_t null_write(struct idesc *desc, const struct iovec *iov, int cnt) {
	int i;
	ssize_t ret_size;

	ret_size = 0;
	for (i = 0; i < cnt; i++) {
		ret_size += iov[i].iov_len;
	}

	return ret_size;
}

static ssize_t null_read(struct idesc *desc, const struct iovec *iov, int cnt) {
	return 0;
}

static const struct idesc_ops null_ops = {
	.close = null_close,
	.id_readv = null_read,
	.id_writev = null_write,
	.fstat     = char_dev_idesc_fstat,
};

CHAR_DEV_DEF(NULL_DEV_NAME, NULL, NULL, &null_ops, NULL);
