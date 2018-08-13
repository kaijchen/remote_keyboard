#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <linux/uinput.h>

#define perr(msg) \
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));

int main(int argc, char *argv[])
{
	int fd;
	struct input_event ie;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <evdev>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	system("/bin/stty -echo");

	if ((fd = open(argv[1], O_RDONLY)) < 0) {
		perr("Failed to open device");
		exit(EXIT_FAILURE);
	}

	for (;;) {
		if (read(fd, &ie, sizeof(ie)) <= 0)
			break;
		write(STDOUT_FILENO, &ie, sizeof(ie));
	}

	return 0;
}
