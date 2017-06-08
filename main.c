// batwarn - (C) 2015-2017 Jeffrey E. Bedard
#include "batwarn.h"
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "config.h"
#include "gamma.h"
#include "version.h"
static void exit_cb(void)
{
	batwarn_set_gamma(BATWARN_GAMMA_NORMAL);
}
__attribute__((noreturn))
static void usage(char * arg0, const char * optstr,
	int sz, const int8_t ec)
{
	{
		uint8_t l = 0;
		while (arg0[++l]);
		write(1, arg0, l);
	}
	write(1, " -", 2);
	write(1, optstr, sz);
	write(1, "\n", 1);
	const char helptext[] =
		"-d\t\tDo not fork a daemon; run in the foreground.\n"
		"-h\t\tShow this usage information.\n"
		"-H\t\tEnable hibernation at critical battery level.\n"
		"-p PERCENT\tSet the warning percent for gamma change.\n"
		"-s\t\tEnable suspend at critical battery level.\n"
		"Copyright 2017, Jeffrey E. Bedard <jefbed@gmail.com>\n";
	write(1, helptext, sizeof(helptext));
	const char * version = batwarn_get_version(&sz);
	write(1, version, sz);
	exit(ec);
}
static uint8_t parse_argv(int argc, char ** argv, uint8_t flags)
{
	int8_t opt;
	const char optstr[] = "dhHp:s";
	while((opt = getopt(argc, argv, optstr)) != -1)
		switch (opt) {
		case 'd': // debug
			flags |= BATWARN_DEBUG;
			break;
		case 'H': // enable hibernate at critical
			flags |= BATWARN_HIBERNATE;
			break;
		case 'p': // warning percentage
			batwarn_set_percent(atoi(optarg));
			break;
		case 's': // enable suspend
			flags |= BATWARN_SUSPEND;
			break;
		case 'h': // help
			usage(*argv, optstr, sizeof(optstr), 0);
		default: // usage
			usage(*argv, optstr, sizeof(optstr), 1);
		}
	return flags;
}
int main(int argc, char **argv)
{
	const uint8_t flags = parse_argv(argc, argv, 0);
	if(!(flags & BATWARN_DEBUG) && (fork() != 0))
		return 0;
	signal(SIGINT, exit);
	signal(SIGTERM, exit);
	atexit(exit_cb);
	batwarn_start_checking(flags);
	return 0;
}
