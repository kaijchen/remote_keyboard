#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
	struct input_event ev;

	while (read(STDIN_FILENO, &ev, sizeof(ev)) == sizeof(ev)) {
		printf("input_event{%d, %d, %d}\n",
				ev.type, ev.code, ev.value);
	}
	exit(EXIT_SUCCESS);
}
