
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include "config.h"
#include "flags.h"
#include "proto.h"
#include "structs.h"
#include "ansi.h"

void         display_version (player * p, char *str)
{
   char        *oldstack = stack;

   banner (p, stack, 0, '-');
   stack = strchr (stack, 0);
#if DUALPLEX
   sprintf (stack, " Serpent ELF Dualplex (%s v%s)\n"
	    " (%d) <--> (%d)\n", RELEASE, VERSION,
	    CONFIG_PORT, DUAL_PORT);
   stack = strchr (stack, 0);
#else
   sprintf (stack, " Serpent ELF (%s v%s)\n", RELEASE, VERSION);
   stack = strchr (stack, 0);
#endif
   sprintf (stack, " Compiled %s", compiled_msg.data);
   stack = strchr (stack, 0);
   banner (p, stack, 0, '-');
   stack = strchr (stack, 0);
   *stack++ = 0;
   writep (p, oldstack);
   stack = oldstack;
   return;
}

void         time_of_day (player * p, char *str)
{
   time_t       t = time (0);
   char         string[50], string2[50];
   char        *oldstack = stack;

   strftime (string, 49, "%A %d %B %Y", localtime (&t));
   strftime (string2, 49, "%I:%M:%S", localtime (&t));
   banner (p, stack, "Time of day", '-');
   stack = strchr (stack, 0);

   sprintf (stack, " Today is %s\n", string);
   stack = strchr (stack, 0);
   sprintf (stack, " Our local time is %s\n", string2);
   stack = strchr (stack, 0);
   banner (p, stack, "System Information", '-');
   stack = strchr (stack, 0);
   sprintf (stack, " Total connections             : %d\n"
	    " Most connections at once      : %d\n"
	    " Current connections           : %d\n"
	    " Pid                           : %d\n"
	    " Uptime                        : %s\n"
	    " Boot date                     : %s\n",
	s_info.logins, s_info.max_logins, s_info.connections, s_info.pid,
	    word_time (s_info.uptime), s_info.boot_date);
   stack = strchr (stack, 0);
   sprintf (stack, " Longest connection            : %s\n"
	    " Set by                        : %s\n",
	    word_time (s_info.on_since), s_info.max_login_name);
   stack = strchr (stack, 0);
   banner (p, stack, "eof", '-');
   stack = strchr (stack, 0);
   *stack++ = 0;
   writep (p, oldstack);
   stack = oldstack;
   return;
}

void         network_traffic (player * p, char *str)
{
   char        *oldstack = stack;

   banner (p, stack, "Network Traffic", '-');
   stack = strchr (stack, 0);

   sprintf (stack, " Total packets received / sent    : %3lu   : %lu\n"
	    " Total bytes received / sent      : %3lu   : %lu\n"
	    " Average pps / bps                : %.1f   : %.1f\n",
	    s_info.in.packets, s_info.out.packets,
	    s_info.in.bytes, s_info.out.bytes, s_info.pps, s_info.bps);
   stack = strchr (stack, 0);
   banner (p, stack, "eof", '-');
   stack = strchr (stack, 0);
   *stack++ = 0;
   writep (p, oldstack);
   stack = oldstack;
   return;
}
