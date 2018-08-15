#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <linux/uinput.h>

#define OLD_INTERFACE
#define DEV_VENDOR 0xdead
#define DEV_PRODUCT 0xbeef
#define DEV_NAME "universal input device"

int create_device()
{
	int fd;
       
	if ((fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK)) < 0) {
		perror("Failed to open /dev/uinput");
		exit(EXIT_FAILURE);
	}

	if (ioctl(fd, UI_SET_EVBIT, EV_KEY) < 0) {
		perror("Failed to set EVBIT");
		exit(EXIT_FAILURE);
	}

	for (int code = 0; code < KEY_MIN_INTERESTING; code++) {
		if (ioctl(fd, UI_SET_KEYBIT, code) < 0) {
			perror("Failed to set KEYBIT");
			exit(EXIT_FAILURE);
		}
	}

	for (int code = BTN_MOUSE; code <= BTN_TASK; code++) {
		if (ioctl(fd, UI_SET_KEYBIT, code) < 0) {
			perror("Failed to set KEYBIT");
			exit(EXIT_FAILURE);
		}
	}

	if (ioctl(fd, UI_SET_EVBIT, EV_REL) < 0) {
		perror("Failed to set EVBIT");
		exit(EXIT_FAILURE);
	}

	if (ioctl(fd, UI_SET_RELBIT, REL_X) < 0) {
		perror("Failed to set RELBIT");
		exit(EXIT_FAILURE);
	}

	if (ioctl(fd, UI_SET_RELBIT, REL_Y) < 0) {
		perror("Failed to set RELBIT");
		exit(EXIT_FAILURE);
	}

	if (ioctl(fd, UI_SET_RELBIT, REL_WHEEL) < 0) {
		perror("Failed to set RELBIT");
		exit(EXIT_FAILURE);
	}

	if (ioctl(fd, UI_SET_RELBIT, REL_HWHEEL) < 0) {
		perror("Failed to set RELBIT");
		exit(EXIT_FAILURE);
	}

	if (ioctl(fd, UI_SET_EVBIT, EV_ABS) < 0) {
		perror("Failed to set EVBIT");
		exit(EXIT_FAILURE);
	}

	if (ioctl(fd, UI_SET_ABSBIT, ABS_X) < 0) {
		perror("Failed to set ABSBIT");
		exit(EXIT_FAILURE);
	}

	if (ioctl(fd, UI_SET_ABSBIT, ABS_Y) < 0) {
		perror("Failed to set ABSBIT");
		exit(EXIT_FAILURE);
	}

#ifdef OLD_INTERFACE
	/* 
	 * Before uinput version 5, there wasn't a dedicated ioctl to set up
	 * a virtual device. Programs supporting older versions of uinput
	 * interface need to fill a uinput_user_dev structure and write it
	 * to the uinput file descriptor to configure the new uinput device.
	 *
	 * New code should not use the old interface but interact with uinput
	 * via ioctl calls, or use libevdev.
	 * 
	 * https://www.kernel.org/doc/html/v4.16/input/uinput.html#uinput-old-interface
	 */
	struct uinput_user_dev uud;

	memset(&uud, 0, sizeof(uud));
	uud.id.bustype = BUS_USB;
	uud.id.vendor = DEV_VENDOR;
	uud.id.product = DEV_PRODUCT;
	snprintf(uud.name, UINPUT_MAX_NAME_SIZE, DEV_NAME);

	if (write(fd, &uud, sizeof(uud)) < 0) {
		perror("Failed to setup device");
		exit(EXIT_FAILURE);
	}
#else
	struct uinput_setup usetup;

	memset(&usetup, 0, sizeof(usetup));
	usetup.id.bustype = BUS_USB;
	usetup.id.vendor = DEV_VENDOR;
	usetup.id.product = DEV_PRODUCT;
	snprintf(usetup.name, UINPUT_MAX_NAME_SIZE, DEV_NAME);

	if (ioctl(fd, UI_DEV_SETUP, &usetup) < 0) {
		perror("Failed to setup device");
		exit(EXIT_FAILURE);
	}
#endif

	if (ioctl(fd, UI_DEV_CREATE) < 0) {
		perror("Failed to create device");
		exit(EXIT_FAILURE);
	}

	return fd;
}

int main(void)
{
	struct input_event ie;

	int fd = create_device();

	for (;;) {
		if (read(STDIN_FILENO, &ie, sizeof(ie)) <= 0)
			break;
		write(fd, &ie, sizeof(ie));
	}

	if (ioctl(fd, UI_DEV_DESTROY) < 0) {
		perror("Failed to destroy device");
		exit(EXIT_FAILURE);
	}

	if (close(fd) < 0) {
		perror("Failed to close fd");
		exit(EXIT_FAILURE);
	}

	return 0;
}
