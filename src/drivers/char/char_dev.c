/*
 * @file
 *
 * @date 28.11.12
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */
#include <errno.h>
#include <stdlib.h>

#include <drivers/char_dev.h>
#include <fs/file_desc.h>
#include <fs/node.h>
#include <fs/vfs.h>
#include <fs/file_operation.h>

#include <util/array.h>

ARRAY_SPREAD_DEF(const struct dev_module, __device_registry);

int char_dev_init_all(void) {
	int ret;
	const struct dev_module *dev_module;

	array_spread_foreach_ptr(dev_module, __device_registry) {
		if (dev_module->init != NULL) {
			ret = dev_module->init();
			if (ret != 0) {
				return ret;
			}
		}
	}

	return 0;
}

/* This stub is supposed to be used when there's no need
 * for device-specific idesc_ops.fstat() */
int char_dev_idesc_fstat(struct idesc *idesc, void *buff) {
	struct stat *sb;

	assert(buff);
	sb = buff;
	memset(sb, 0, sizeof(struct stat));
	sb->st_mode = S_IFCHR;

	return 0;
}

#if 0
static struct idesc *char_dev_open(struct node *node, struct file_desc *file_desc, int flags) {
	struct dev_module *dev = node->nas->node_fi;
}

static struct file_operations char_file_ops = {
	.open = char_dev_open,
};
#endif

int char_dev_register(const char *name, const struct file_operations *ops) {
	struct path  node;
	struct nas *dev_nas;

	if (vfs_lookup("/dev", &node)) {
		return -ENOENT;
	}

	if (node.node == NULL) {
		return -ENODEV;
	}

	vfs_create_child(&node, name, S_IFCHR | S_IRALL | S_IWALL, &node);
	if (!(node.node)) {
		return -1;
	}

	dev_nas = node.node->nas;
	dev_nas->fs = filesystem_create("empty");
	if (dev_nas->fs == NULL) {
		return -ENOMEM;
	}

	dev_nas->fs->file_op = ops;

	return 0;
}
