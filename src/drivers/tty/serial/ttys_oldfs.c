/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    09.08.2013
 */
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <util/err.h>
#include <util/indexator.h>

#include <mem/misc/pool.h>
#include <drivers/char_dev.h>
#include <drivers/serial/uart_device.h>
#include "idesc_serial.h"
#include <fs/file_operation.h>
#include <fs/node.h>
//#include <fs/file_desc.h>
#include <util/err.h>

#if 0
static struct idesc *uart_fsop_open(struct node *node, struct file_desc *file_desc, int flags)  {
	struct uart *uart;
	struct idesc *idesc;
	int res;

	uart = uart_dev_lookup(node->name);
	if (!uart) {
		return err_ptr(ENOENT);
	}
	idesc = idesc_serial_create(uart, flags);
	if (err(idesc)) {
		return idesc;
	}
	res = uart_open(uart);
	if (res) {
		return err_ptr(-res);
	}

	return idesc;
}

/*
const struct file_operations ttys_fops = {
	.open = uart_fsop_open,
};
*/
#endif

#define SERIAL_POOL_SIZE OPTION_GET(NUMBER, serial_quantity)
POOL_DEF(cdev_serials_pool, struct dev_module, SERIAL_POOL_SIZE);

int ttys_register(const char*name, void *dev_info) {
	struct dev_module *cdev;

	cdev = pool_alloc(&cdev_serials_pool);
	if (!cdev) {
		return -ENOMEM;
	}
	memset(cdev, 0, sizeof(*cdev));
	memcpy(cdev->name, name, sizeof(cdev->name));
	cdev->dev_priv = dev_info;

	return char_dev_register(cdev);
}
