//////////////////////////////////////////////////////////////////
// File: misc.c                                                 //
//       Contains basic functions.                              //
//////////////////////////////////////////////////////////////////
// Copyright (c) 1998.  Serpent Development                     //
// Regents to Douglas Hanks.                                    //
//////////////////////////////////////////////////////////////////
// You may reach Serpent Development via email at:              //
// Douglas Hanks: ima@realms.org                                //
// Tom Malcom   : tom@ackphft.matsu.alaska.edu                  //
//////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "flags.h"
#include "proto.h"
#include "structs.h"

void         show_exits (player * p, char *str)
{
   if (p->flags & SHOW_EXITS)
   {
      tell_player (p, "You will not see exits now.\n");
      p->flags &= ~SHOW_EXITS;
   }
   else
   {
      tell_player (p, "You will now see exits.\n");
      p->flags |= SHOW_EXITS;
   }
   return;
}

void         show_maps (player * p, char *str)
{
   if (p->flags & SHOW_MAPS)
   {
      tell_player (p, "You will not see maps now.\n");
      p->flags &= ~SHOW_MAPS;
   }
   else
   {
      tell_player (p, "You will now see maps.\n");
      p->flags |= SHOW_MAPS;
   }
   return;
}

void         c_quit (player * p, char *str)
{
   tell_player (p, " \n\n --\n"
		" Bytes/packets received from you  : %d/%d\n"
		" Bytes/packets sent to you        : %d/%d\n"
		" Commands executed                : %d\n"
		" Your file descriptor             : %d\n"
		" Your dns                         : %s\n"
		" --\n\n", p->sock.in.bytes, p->sock.in.packets,
		p->sock.out.bytes, p->sock.out.packets, p->commands_out,
		p->sock.fd, p->sock.host);
   save_player (p, 0);
   p->flags |= P_LOGOUT;
   return;
}

void         reload (player * p, char *str)
{
   writep (p, " Reloading files ...\n");
   init_dfiles ();
   writep (p, " Reloading dynamic ELF module commands ..\n");
   init_command_modules ();
   vlog ("reload", "%s reloaded the system.\n", p->name);
   return;
}

void command_help(player *p, char *str)
{
   dfile help;
   char *oldstack = stack;
   char  location[80];
   
   sprintf(location, "%slib/help/default.hlp", SERVER_ROOT);
   if(!*str)
     help = load_dfile(location);
   else
     {
	sprintf(location, "%slib/help/%s.hlp", SERVER_ROOT, str);
	help = load_dfile(location);
     }
   if(!help.data || help.size < 2)
     {
	writep(p," Couldn't find any help on that topic.\n");
	return;
     }
   banner(p, stack, 0, '-');
   stack = strchr(stack, 0);
   strcpy(stack, help.data);
   stack = strchr(stack, 0);
   *stack++='\n';
   banner(p, stack, 0, '-');
   stack = strchr(stack, 0);
   *stack++=0;
   writep(p, oldstack);
   stack = oldstack;
   return;
}
