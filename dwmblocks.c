#include "config.h"

static char statusbar[LENGTH(blocks)][CMDLENGTH] = { 0 };
static char statusstr[2][STATUSLENGTH];
static int statusContinue = 1;

/* Opens process cmd and stores output in output */
void getcmd(const Block *block, char *output) {
	strcpy(output, block->icon);
	FILE *cmdf = popen(block->command, "r");
	if (!cmdf) return;

	int i = strlen(block->icon);
	fgets(output + i, CMDLENGTH - i - (strlen(delim) + 1), cmdf);
	i = strlen(output);
	if (i == 0) {
		/* Return if block and command output are both empty */
		pclose(cmdf);
		return;
	}
	/* Only chop off newline if one is present at the end */
	i = output[i - 1] == '\n' ? i - 1 : i;
	if (delim[0] != '\0') {
		strncpy(output + i, delim, (strlen(delim) + 1));
	} else {
		output[i++] = '\0';
    }
	pclose(cmdf);
}

void getcmds(int time) {
	const Block *current;
	for (unsigned int i = 0; i < LENGTH(blocks); ++i) {
		current = blocks + i;
		if ((current->interval != 0
            && time % current->interval == 0)
            || time == -1)
        {
			getcmd(current,statusbar[i]);
        }
	}
}

void getsigcmds(unsigned int signal) {
	const Block *current;
	for (unsigned int i = 0; i < LENGTH(blocks); ++i) {
		current = blocks + i;
		if (current->signal == signal) {
			getcmd(current,statusbar[i]);
        }
	}
}

void setupsignals() {
#ifndef __OpenBSD__
    /* Initialize all real time signals with dummy handler */
    for (int i = SIGRTMIN; i <= SIGRTMAX; ++i) {
        signal(i, dummysighandler);
    }
#endif

	for (unsigned int i = 0; i < LENGTH(blocks); ++i) {
		if (blocks[i].signal > 0) {
			signal(SIGMINUS + blocks[i].signal, sighandler);
        }
	}
}

int getstatus(char *str, char *last) {
	strcpy(last, str);
	str[0] = '\0';

	for (unsigned int i = 0; i < LENGTH(blocks); ++i) {
		strcat(str, statusbar[i]);
    }
	str[strlen(str) - strlen(delim)] = '\0';
	return strcmp(str, last);
}

#ifndef NO_X
void setroot() {
    /* Only set root if text has changed */
	if (!getstatus(statusstr[0], statusstr[1])) {
		return;
    }
	XStoreName(dpy, root, statusstr[0]);
	XFlush(dpy);
}

int setupX() {
	dpy = XOpenDisplay(NULL);
	if (!dpy) {
		fprintf(stderr, "dwmblocks: Failed to open display\n");
		return 0;
	}
	screen = DefaultScreen(dpy);
	root = RootWindow(dpy, screen);
	return 1;
}
#endif

void pstdout() {
    /* Only write out if text has changed */
	if (!getstatus(statusstr[0], statusstr[1])) {
		return;
    }
	printf("%s\n",statusstr[0]);
	fflush(stdout);
}

void statusloop() {
	setupsignals();
	int i = 0;
	getcmds(-1);
	while (true) {
		getcmds(i++);
		writestatus();
		if (!statusContinue) {
			break;
        }
		sleep(1);
	}
}

#ifndef __OpenBSD__
/* this signal handler should do nothing */
void dummysighandler(int signum) {
    return;
}
#endif

void sighandler(int signum) {
	getsigcmds(signum - SIGPLUS);
	writestatus();
}

void termhandler(int signal) {
    (void)signal;
	statusContinue = 0;
}

int main(int argc, char **argv) {
    /* Handle command line arguments */
	for (int i = 0; i < argc; i++) {
		if (!strcmp("-d", argv[i])) {
			strncpy(delim, argv[++i], (strlen(delim) + 1));
        } else if (!strcmp("-p",argv[i])) {
			writestatus = pstdout;
        }
	}
#ifndef NO_X
	if (!setupX()) return 1;
#endif
	delim[strlen(delim)] = '\0';
	signal(SIGTERM, termhandler);
	signal(SIGINT,  termhandler);
	statusloop();
#ifndef NO_X
	XCloseDisplay(dpy);
#endif
	return 0;
}
