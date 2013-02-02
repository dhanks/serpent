//////////////////////////////////////////////////////////////////
// File: login.c                                                //
//       Contains proper login procedures.                      //
//////////////////////////////////////////////////////////////////
// Copyright (c) 1998.  Serpent Development                     //
// Regents to Douglas Hanks.                                    //
//////////////////////////////////////////////////////////////////
// You may reach Serpent Development via email at:              //
// Douglas Hanks: ima@realms.org                                //
// Tom Malcom   : tom@ackphft.matsu.alaska.edu                  //
//////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdio.h>
#include <sys/utsname.h>

#include "config.h"
#include "flags.h"
#include "proto.h"
#include "structs.h"

void         got_login (player *, char *);
void         login_prompt (player *);
void         enter_arena (player *, char *);
void         show_motd (player *, char *);

void         view_motd (player * p, char *str)
{
   tell_player (p, "\n");
   tell_player (p, motd_msg.data);
   return;
}

void         login_timeout (player * p, char *dummy)
{
   writep (p, "Login timed out after 60 seconds.\n");
   p->flags |= P_LOGOUT;

   return;
}

void         agree_dis (player * p, char *str)
{
   if (!strcasecmp (str, "agree"))
   {
      writep (p, "Terms and conditions are accepted.\n");
      writep (p, motd_msg.data);
      promptp (p, " [Hit <RETURN> to continue.]");
      p->func = show_motd;
   }
   else if (!strcasecmp (str, "reject"))
   {
      writep (p, "Closing your connection immediately.\n");
      p->flags |= P_LOGOUT;
   }
   else
   {
      promptp (p, " Please type \"agree\" or \"reject\" to these terms: ");
      p->func = agree_dis;
   }
}

void         enter_terminal (player * p, char *str)
{
   if (!*str)
      c_terminal (p, "vt100");
   else if (!strncasecmp (str, "vt100", strlen (str)))
      c_terminal (p, "vt100");
   else if (!strncasecmp (str, "ansi", strlen (str)))
      c_terminal (p, "ansi");
   else
      c_terminal (p, "none");
   writep (p, dis_msg.data);
   promptp (p, " Please type \"agree\" or \"reject\" to these terms: ");
   p->func = agree_dis;
}

void         screen_two (player * p, char *str)
{
   writep (p, terminals_msg.data);
   promptp (p, " Enter your selection[vt100]: ");
   p->func = enter_terminal;
   return;
}

void         show_motd (player * p, char *dummy)
{
   enter_arena (p, 0);
   return;
}


void         enter_arena (player * p, char *dummy)
{
   int          i, match = 0;

   p->flags |= P_PROMPT;
   move_to (p, mainroom);
   command_look (p, 0);
   channel = CHAN_SYSTEM;
   tell_player (p, "\n ~o~ Welcome to serpent %s! ~o~\n", p->name);
   if (p->flags & P_RECON)
   {
      tell_player (p, "\n [ reconnection complete. ]\n");
      p->flags &= ~P_RECON;
      vlog ("connections", "%s has reconnected.\n", p->name);
      if (p->dual == 1)
	 vwriter (p, 0, p->room, "[%s %s][dualplex two]\n", p->name, p->reconmsg);
      else
	 vwriter (p, 0, p->room, "[%s %s][dualplex one]\n", p->name, p->reconmsg);
   }
   else
   {
      channel = CHAN_SYSTEM;
      vlog ("connections", "%s has connected - %s (%s:%d)\n", p->name, p->sock.host, p->sock.ip, p->sock.port);
      if (p->dual == 1)
	 vwriter (p, 0, p->room, "[%s %s][dualplex two]\n", p->name, p->conmsg);
      else
	 vwriter (p, 0, p->room, "[%s %s][dualplex one]\n", p->name, p->conmsg);
   }
   for (i = 0; root_users[i].key; i++)
      if (!strcasecmp (p->name, root_users[i].key))
      {
	 match = 1;
	 tell_player (p, " Welcome, Administrator.\n");
	 p->residency |= RESIDENT;
	 p->residency |= BASE;
	 p->residency |= PSU;
	 p->residency |= SU;
	 p->residency |= LOWER_ADMIN;
	 p->residency |= ADMIN;
	 p->residency |= HCADMIN;
	 p->residency |= CODER;
	 p->residency |= SYSOP;
	 vlog ("admin", "%s connects from %s %s:%d\n", p->name, p->sock.host, p->sock.ip,
	       p->sock.port);
      }

   if (match == 0)
   {
      if (!(p->residency & RESIDENT))
	 tell_player (p, " [ you raise to level 1. ]\n");
      p->residency |= BASE;
   }
   if (system_flags & NO_NEWBIES)
      if (p->residency & PSU)
	 writep (p, " -=> We're currently closed to newbies.\n");
   if (system_flags & TEMP_CLOSED)
      writep (p, " -=> We're temporarily closed due to repair.\n");
   channel = 0;
   p->flags |= HERE;
   p->func = 0;
   return;
}

void         got_login_two (player * p)
{
   player      *scan;

   for (scan = p_top; scan; scan = scan->p_next)
   {
      if (!strcasecmp (scan->name, p->name) && scan != p)
      {
	 tell_player (scan, "Your mind slowly drifts away as you are reconnected from %s:%d\n", p->sock.ip, p->sock.port);
	 scan->flags &= ~HERE;
	 c_quit (scan, 0);
	 p->flags |= P_RECON;
	 writep (p, "[ detected a reconnection. ]\n");
      }
   }
   enter_arena (p, 0);
   return;
}

void         get_pass (player * p, char *str)
{
   if (!strcmp (str, p->password))
   {

      got_login_two (p);
      return;
   }
   else
   {

      writep (p, " Password incorrect.\n\n");
      login_prompt (p);
      return;
   }
}
void         got_login (player * p, char *str)
{
   char        *fault;
   player      *scan;

   str = trim_end (str);


   if ((fault = bad_name (str)))
   {
      writep (p, fault);
      login_prompt (p);
      return;
   }
   lower_case (str);
   sprintf (p->lower_name, str);
   if (player_load (p))
   {
      if (system_flags & TEMP_CLOSED)
	 if (!(p->residency & SU))
	 {
	    writep (p, " Sorry, we're currently closed to connections.  Try again later.\n\n");
	    p->flags |= P_LOGOUT;
	    return;
	 }
      promptp (p, "password: ");
      p->func = get_pass;
      return;
   }

   if (!strcasecmp (str, "quit"))
   {
      p->flags |= P_LOGOUT;
      return;
   }

   if (p->flags & P_RECON)
   {
      enter_arena (p, 0);
      return;
   }
   if (system_flags & NO_NEWBIES)
   {
      writep (p, " Sorry, but we're currently closed to newbies.  Try again later.\n\n");
      p->flags |= P_LOGOUT;
      return;
   }
   for (scan = p_top; scan; scan = scan->p_next)
   {
      if (!strcasecmp (str, scan->name))
      {
	 writep (p, " There is already someone on the program with that name.\n\n");
	 login_prompt (p);
	 return;
      }
   }
   strncpy (p->name, str, P_NAME_LEN);
   strncpy (p->lower_name, p->name, P_NAME_LEN);
   lower_case (p->lower_name);
   writep (p, "\n");
   writep (p, screen_one_msg.data);
   promptp (p, "[Hit <RETURN> to continue] ");

   p->func = screen_two;
   return;
}

void         login_prompt (player * p)
{
   promptp (p, "serpent login:");
   p->func = got_login;
   p->timeout = 60;
   p->timeout_func = login_timeout;
   return;
}


void         login_screen (player * p)
{
   if (system_flags & LOCKOUT)
   {
      writep (p, " Sorry, but we're currently closed to all connections.\n\n");
      p->flags |= P_LOGOUT;
      return;
   }
   if (shutdown_count > 0)
   {
      tell_player (p, " Sorry, there is a system reboot scheduled in %s.  Please try again later.\n\n", word_time (shutdown_count));
      p->flags |= P_LOGOUT;
      return;
   }
   writep (p, connect_msg.data);
   tell_player (p, " Serpent is running on %s Version %s on %s hardware.\n Nodename is %s.\n\n", uts.sysname, uts.release, uts.machine, uts.nodename);
   login_prompt (p);
   return;
}
