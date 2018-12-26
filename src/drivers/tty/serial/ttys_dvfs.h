/**
 * @file
 *
 * @date 21.04.2016
 * @author Anton Bondarev
 */

#ifndef TTYS_H_
#define TTYS_H_

#include <fs/idesc.h>
#include <drivers/tty.h>
#include <drivers/char_dev.h>

struct uart;
struct tty_uart {
	struct idesc idesc;
	struct tty tty;
	struct uart *uart;
};

extern int ttys_open(struct dev_module *mod, void *dev_priv);

#define TTYS_DEF(name, uart) \
		CHAR_DEV_DEF(name, &ttys_open, NULL, NULL, uart)

#endif /* TTYS_H_ */
