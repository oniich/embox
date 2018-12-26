/**
 * @file
 * @brief Declarations for char dev based on dvfs
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-10-05
 */

#ifndef CHAR_DEV_DVFS_H_
#define CHAR_DEV_DVFS_H_

#include <drivers/device.h>
#include <fs/dvfs.h>

#define CHAR_DEV_DEF(chname, open_fn, close_fn, idesc_op, priv) \
	ARRAY_SPREAD_DECLARE(const struct dev_module, __device_registry); \
	ARRAY_SPREAD_ADD(__device_registry, { \
			.name = chname, \
			.dev_priv = priv, \
			.dev_iops = idesc_op, \
			.open = open_fn, \
			.close = close_fn, \
			 })

extern int char_dev_register(struct dev_module *cdev);

static inline struct dev_module *idesc_to_dev_module(struct idesc *desc) {
	struct file *f = mcast_out(desc, struct file, f_idesc);

	return mcast_out(f, struct dev_module, dev_file);
}

extern int char_dev_idesc_fstat(struct idesc *idesc, void *buff);

#endif /* CHAR_DEV_DVFS_H_ */
