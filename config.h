#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#ifndef NO_X
#include <X11/Xlib.h>
#endif

#ifdef __OpenBSD__
#define SIGPLUS			(SIGUSR1 + 1)
#define SIGMINUS		(SIGUSR1 - 1)
#else
#define SIGPLUS			SIGRTMIN
#define SIGMINUS		SIGRTMIN
#endif
#define LENGTH(X)       (sizeof(X) / sizeof (X[0]))
#define CMDLENGTH		50
#define MIN(a, b) ((a < b) ? a : b)
#define STATUSLENGTH (LENGTH(blocks) * CMDLENGTH + 1)

typedef struct {
	char* icon;
	char* command;
	unsigned int interval;
	unsigned int signal;
} Block;

#ifndef __OpenBSD__
void dummysighandler(int num);
#endif
void sighandler(int num);
void getcmds(int time);
void getsigcmds(unsigned int signal);
void setupsignals();
void sighandler(int signum);
int getstatus(char *str, char *last);
void statusloop();
void termhandler();
void pstdout();
#ifndef NO_X
void setroot();
static void (*writestatus) () = setroot;
static int setupX();
static Display *dpy;
static int screen;
static Window root;
#else
static void (*writestatus) () = pstdout;
#endif

/*** User configuration ***/

static const Block blocks[] = {
	/* Icon          Command                                                    Update Interval  Update Signal */
    { " 💾 Mem: ",   "free -h | awk '/^Mem/ { print $3\"/\"$2 }' | sed s/i//g",	30,		         0 },
    { " 📅 Date: ",  "date '+%b %d (%a)'",					                    60,		         0 },
    { " 🕛 Time: ",  "date '+%I:%M %p '",					                    60,	             0 },
};

/*
   Sets delimeter between status commands.
   A NULL character ('\0') means no delimeter.
*/
static char delim[] = " |";
static unsigned int delimLen = 5;
