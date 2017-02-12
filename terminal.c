#include "terminal.h"
#include <sys/termios.h>
#include <stdlib.h>
#include <string.h>

static struct termios orig_termios;
static void terminal_reset_mode();

static void terminal_reset_mode() {
	tcsetattr(0, TCSANOW, &orig_termios);
}

void terminal_conio_mode() {
	struct termios new_termios;

	tcgetattr(0, &orig_termios);
	memcpy(&new_termios, &orig_termios, sizeof(new_termios));

	atexit(terminal_reset_mode);
	cfmakeraw(&new_termios);
	tcsetattr(0, TCSANOW, &new_termios);
}