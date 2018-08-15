#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAXFD 32
#define BUFSZ 32

int main(int argc, char *argv[])
{
	int fd[MAXFD];
	int nfd;
	fd_set rfds;
	int retval;
	ssize_t n;
	struct input_event buf[BUFSZ];

	if (argc < 2) {
		fprintf(stderr, "Usage: %s evdev [evdev ...]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	system("/bin/stty -echo");

	for (int i = 1; i < argc; i++) {
		if ((fd[i] = open(argv[i], O_RDONLY | O_NONBLOCK)) < 0) {
			perror("open()");
			exit(EXIT_FAILURE);
		}
	}

	nfd = fd[argc - 1] + 1;

	for (;;) {
		FD_ZERO(&rfds);
		for (int i = 1; i < argc; i++)
			FD_SET(fd[i], &rfds);
		if ((retval = select(nfd, &rfds, NULL, NULL, NULL)) == -1) {
			perror("select()");
			exit(EXIT_FAILURE);
		} else if (retval) {
			for (int i = fd[1]; i < nfd; i++)
				if (FD_ISSET(i, &rfds)) {
					n = read(i, buf, sizeof(buf));
					if (n < 0) {
						perror("read()");
						exit(EXIT_FAILURE);
					} else if (n) {
						write(STDOUT_FILENO, buf, n);
					}
				}
		}
	}
	exit(EXIT_SUCCESS);
}
