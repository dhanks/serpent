//////////////////////////////////////////////////////////////////
// File: commands.c                                             //
//////////////////////////////////////////////////////////////////
// Copyright (c) 1998.  Serpent Development                     //
// Regents to Douglas Hanks.                                    //
//////////////////////////////////////////////////////////////////
// You may reach the Serpent Development via email at:          //
// Douglas Hanks: ima@realms.org                                //
// Tom Malcom   : tom@ackphft.matsu.alaska.edu                  //
//////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdlib.h>

#include "../src/config.h"
#include "../src/flags.h"
#include "../src/proto.h"
#include "../src/structs.h"

void         lockout (player * p, char *str)
{
   if (!*str)
   {
      writep (p, " Format : lockout <reason>\n");
      return;
   }
   if (system_flags & LOCKOUT)
   {
      su_wall ("-=> %s unlocks the system with reason: %s\n", p->name,
str);
      vlog ("lockout", "-=> %s unlocks the system with reason: %s\n",
                        p->name, str);
      system_flags &= ~LOCKOUT;
   }
   else
   {
      su_wall ("-=> %s locks the system with the reason: %s\n", p->name,
str);
      vlog ("lockout", "-=> %s locks the system with the reason: %s\n",
                        p->name, str);
      system_flags |= LOCKOUT;
   }
   return;
}

void         ressies (player * p, char *str)
{
   if (!*str)
   {
      writep (p, " Format : ressies <reason>\n");
      return;
   }
   if (system_flags & TEMP_CLOSED)
   {
      su_wall ("-=> %s opens the system with reason: %s\n", p->name, str);
      vlog ("ressies", "-=> %s opens the system with reason: %s\n",
                         p->name, str);
      system_flags &= ~TEMP_CLOSED;
   }
   else
   {
      su_wall ("-=> %s closes the system with the reason: %s\n", p->name,
str);
      vlog ("ressies", "-=> %s closes the system with the reason: %s\n",
                        p->name, str);
      system_flags |= TEMP_CLOSED;
   }
   return;
}

void         close_to_newbies (player * p, char *str)
{
   if (!*str)
   {
      writep (p, " Format : newbies <reason>\n");
      return;
   }
   if (system_flags & NO_NEWBIES)
   {
      su_wall ("-=> %s opens the system to newbies with reason: %s\n",
               p->name, str);
      vlog ("newbies", "-=> %s opens the system to newbies with reason:
%s\n",
                        p->name, str);
      system_flags &= ~NO_NEWBIES;
   }
   else
   {
      su_wall ("-=> %s closes the system to newbies with the reason:
%s\n",
                p->name, str);
  vlog ("newbies", "-=> %s closes the system to newbies with the reason:
%s\n",
                    p->name, str);
      system_flags |= NO_NEWBIES;
   }
   return;
}

void         command_grant (player * p, char *str)
{
   char        *param;
   int          i, match = 0;
   player      *p2;

   if (!*str)
   {
      writep (p, " Usage : grant <name> <priv> [list]\n");
      return;
   }
   if (!strcasecmp (str, "list"))
   {
      writep (p, " -- Levels --\n");
      for (i = 0; defined_levels[i].key; i++)
         tell_player (p, " + %s\n", defined_levels[i].key);
      writep (p, " ------------\n");
      return;
   }

   param = next_space (str);
   *param++ = 0;
   p2 = find_player_name (str);

   if (!p2)
   {
      p2 = malloc_player ();
      match = 1;
      lower_case (str);
      sprintf (p2->lower_name, str);
      if (!player_load (p2))
      {
         writep (p, " Couldn't find that player.\n");
         free (p2);
         return;
      }

   }
   for (i = 0; defined_levels[i].key; i++)
   {
      if (!strncasecmp (defined_levels[i].key, param, strlen (param)))
      {
         p2->residency |= defined_levels[i].flag;
         tell_player (p, " Granted %s %s\n", p2->name,
defined_levels[i].key);
         if (match == 1)
         {
            save_player (p2, 0);
            free (p2);
         }
         return;
      }
   }
   if (match == 1)
      free (p2);
   writep (p, " Can't find that permission.  Use \"grant list\" for a
list.\n");   return;
}

void         command_remove (player * p, char *str)
{
   char        *param;
   int          i, match = 0;
   player      *p2;

   if (!*str)
   {
      writep (p, " Format : remove <name> <priv> [list]\n");
      return;
   }

   if (!strcasecmp (str, "list"))
   {
      writep (p, " -- Levels --\n");
      for (i = 0; defined_levels[i].key; i++)
         tell_player (p, " + %s\n", defined_levels[i].key);
      writep (p, " ------------\n");
      return;
   }

   param = next_space (str);
   *param++ = 0;
   p2 = find_player_name (str);
   if (!p2)
   {
      match = 1;
      p2 = malloc_player ();
      lower_case (str);
      sprintf (p2->lower_name, str);
      if (!player_load (p2))
      {
         writep (p, " Couldn't find that player.\n");
         free (p2);
         return;
      }
   }
   for (i = 0; defined_levels[i].key; i++)
   {
      if (!strncasecmp (defined_levels[i].key, param, strlen (param)))
      {
         p2->residency &= ~defined_levels[i].flag;
tell_player (p, " Removed %s from %s\n", defined_levels[i].key, p2->name);
         if (match == 1)
         {
            save_player (p2, 0);
            free (p2);
         }
         return;
      }
   }
   if (match == 1)
      free (p2);
   writep (p, " Can't find that permission.  Use \"grant list\" for a
list.\n");   return;
}

void         shutdown_system (player * p, char *str)
{
   if (!*str)
   {
      if (shutdown_count == -1)
         writep (p, " No shutdown is currently scheduled.\n");
      else
         tell_player (p, " Next shutdown is scheduled in %s.\n",
                      word_time (shutdown_count));
      return;
   }

   if (atoi (str) < 0)
   {
      writep (p, " Shutdown aborted.\n");
      shutdown_count = -1;
      return;
   }

   if (atoi (str) == 1)
      shutdown_count = 1;
   else
      shutdown_count = (atoi (str) * 60);
   tell_player (p, " Shutdown set for %s.\n", word_time (shutdown_count));
   return;
}

void         splat (player * p, char *str)
{
   char         temp[80];
   player      *p2;

   if (!*str)
   {
      writep (p, "Format : splat <player>\n");
      return;
   }

   p2 = find_player_name (str);
   if (!p2)
   {
      writep (p, "I don't see that player here ..\n");
      return;
   }

   sprintf (temp, " -=> %s suddenly disappears, leaving no trace.\n",
p2->name);   writea (temp);
   c_quit (p2, 0);
   vlog ("splat", "%s splats %s\n", p->name, p2->name);
   return;
}


void         set_gender (player * p, char *str)
{
   if (!*str)
   {
      writep (p, " Format : gender <M, F, N>\n");
      return;
   }

   if (!strcasecmp (str, "M"))
   {
      p->gender &= ~FEMALE;
      p->gender &= ~NEUTER;
      p->gender |= MALE;
   }
   else if (!strcasecmp (str, "F"))
   {
      p->gender |= FEMALE;
      p->gender &= ~NEUTER;
      p->gender &= ~MALE;
   }
   else if (!strcasecmp (str, "N"))
   {
      p->gender &= ~FEMALE;
      p->gender |= NEUTER;
      p->gender &= ~MALE;
   }
   else
   {
      writep (p, " That option not recongized.\n");
      return;
   }
   tell_player (p, " You set your gender so you are now a %s.\n", heshe (p));
   return;
}


void         examine (player * p, char *str)
{
   player      *scan;
   char        *oldstack = stack;
   int          match = 0;

   if (!*str)
      scan = p;
   if (*str)
      scan = find_player_name (str);

   if (!scan)
   {
      scan = malloc_player ();
      lower_case (str);
      sprintf (scan->lower_name, str);
      if (!player_load (scan))
      {
	 writep (p, " Couldn't find that player.\n");
	 free (scan);
	 return;
      }
      match = 1;
   }

   banner (p, stack, 0, '-');
   stack = strchr (stack, 0);
   sprintf (stack, " %s %s\n", scan->name, scan->title);
   stack = strchr (stack, 0);
   banner (p, stack, 0, '-');
   stack = strchr (stack, 0);
   if (scan->description[0])
   {
      sprintf (stack, " %s\n", scan->description);
      stack = strchr (stack, 0);
      banner (p, stack, 0, '-');
      stack = strchr (stack, 0);
   }
   if (scan == p)
   {
      sprintf (stack, " Entermsg      : %s\n"
	       " Disconnectmsg : %s\n"
	       " Connectmsg    : %s\n"
	       " Reconnectmsg  : %s\n", scan->entermsg, scan->disconmsg,
	       scan->conmsg, scan->reconmsg);
      stack = strchr (stack, 0);
   }
   banner (p, stack, 0, '-');
   stack = strchr (stack, 0);
   sprintf (stack, " %s has been logged in for %s.\n", scan->name,
	    word_time (scan->on_since));
   stack = strchr (stack, 0);
   sprintf (stack, " That is from %s.\n", string_time (p->login_time));
   stack = strchr (stack, 0);
   sprintf (stack, " packets received / sent : %lu / %lu\n"
	    " bytes received / sent   : %lu / %lu\n"
	    " commands excuted        : %d\n", scan->sock.in.packets,
       scan->sock.out.packets, scan->sock.in.bytes, scan->sock.out.bytes,
	    scan->commands_out);
   stack = strchr (stack, 0);
   banner (p, stack, 0, '-');
   stack = strchr (stack, 0);
   sprintf (stack, " Email   : %s\n"
	    " Url     : %s\n", scan->email, scan->url);
   stack = strchr (stack, 0);
#if DUALPLEX
   if (scan->dual == 0)
      strcpy (stack, " Dualplex: One\n");
   else
      strcpy (stack, " Dualplex: Two\n");
   stack = strchr (stack, 0);
#endif
   banner (p, stack, 0, '-');
   stack = strchr (stack, 0);
   sprintf (stack, " Socket : %d %s [%s]\n", scan->sock.fd, scan->sock.host, scan->sock.ip);
   stack = strchr (stack, 0);
   sprintf (stack, " Input : Bytes: %lu \t\t Packets: %lu\n", scan->sock.in.bytes, scan->sock.in.packets);
   stack = strchr (stack, 0);
   sprintf (stack, " Output: Bytes: %lu \t\t Packets %lu\n", scan->sock.out.bytes, scan->sock.out.packets);
   stack = strchr (stack, 0);
   sprintf (stack, " Buffered Output: \t\t B Size %lu\tAge: %lu\n", scan->buffer.size, scan->buffer.age);
   stack = strchr (stack, 0);
   banner (p, stack, 0, '-');
   stack = strchr (stack, 0);
   *stack++ = 0;
   writep (p, oldstack);
   stack = oldstack;
   if (match == 1)
      free (scan);
   return;
}

void         do_shout (player * p, char *str)
{
   player      *scan;
   char        *mid, *p_mid;

   if (!*str)
   {
      writep (p, "Format : shout <msg>\n");
      return;
   }
   switch (asks (str))
   {
   case '?':
      mid = "shouts asking";
      p_mid = "shout asking";
      break;
   case '!':
      mid = "yells";
      p_mid = "yell";
      break;
   default:
      mid = "shouts";
      p_mid = "shout";
   }
   channel = CHAN_SHOUTS;
   for (scan = p_top; scan; scan = scan->p_next)
      if (scan != p && scan->flags & HERE && scan->room)
	 tell_player (scan, "%s %s, '%s'\n", p->name, mid, str);
   channel = 0;
   tell_player (p, "You %s, '%s'\n", p_mid, str);
   return;
}

void         redo_command (player * p, char *str)
{
   if (*str)
   {
      do_shout (p, str);
      return;
   }

   if (p->redo_buffer[0])
      run_command (p, p->redo_buffer);
   else
      tell_player (p, " There is nothing in your redo buffer, tosser!\n");
   return;
}

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


void         c_say (player * p, char *str)
{
   char        *mid;

   if (!*str)
   {
      writep (p, "Format : say <msg>\n");
      return;
   }
   switch (asks (str))
   {
   case '?':
      mid = "ask";
      break;
   case '!':
      mid = "exclaim";
      break;
   default:
      mid = "say";
   }
   channel = CHAN_ROOM;
   vwriter (p, 0, p->room, "%s %ss \'%s\'\n", p->name, mid, str);
   channel = 0;
   vwritep (p, "You %s \'%s\'\n", mid, str);
   return;
}

void         c_emote (player * p, char *str)
{
   char        *mid = " ";

   if (!*str)
   {
      writep (p, " Format : emote <emotion>\n");
      return;
   }
   if (*str == 39 || *str == 44)
      mid = "";
   channel = CHAN_ROOM;
   vwriter (p, 0, p->room, "%s%s%s\n", p->name, mid, str);
   channel = 0;
   vwritep (p, "You emote: %s%s%s\n", p->name, mid, str);
   return;
}

void         c_think (player * p, char *str)
{
   if (!*str)
   {
      writep (p, "Format : think <idea>\n");
      return;
   }
   channel = CHAN_ROOM;
   vwriter (p, 0, p->room, "%s thinks . o O ( %s )\n", p->name, str);
   channel = 0;
   vwritep (p, "You think . o O ( %s )\n", str);
   return;
}

void         admin_emote (player * p, char *str)
{
   char        *mid = " ";

   if (!*str)
   {
      writep (p, " Format : a;<emotion>\n");
      return;
   }
   if (*str == 39 || *str == 44)
      mid = "";
   channel = CHAN_ADMIN;
   ad_wall ("%s%s%s\n", p->name, mid, str);
   channel = 0;
   return;
}

void         admin_think (player * p, char *str)
{
   if (!*str)
   {
      writep (p, " Format : a~<thought>\n");
      return;
   }
   channel = CHAN_ADMIN;
   ad_wall ("%s thinks . o O ( %s )\n", p->name, str);
   channel = 0;
   return;
}

void         admin_say (player * p, char *str)
{
   char        *mid;

   if (!*str)
   {
      writep (p, "Format : a'<msg>\n");
      return;
   }
   switch (asks (str))
   {
   case '?':
      mid = "ask";
      break;
   case '!':
      mid = "exclaim";
      break;
   default:
      mid = "say";
   }
   channel = CHAN_ADMIN;
   ad_wall ("%s %ss \'%s\'\n", p->name, mid, str);
   channel = 0;
   return;
}

void         tell (player * p, char *str)
{
   player      *p2;
   char        *msg, *mid;

   msg = next_space (str);
   if (!*msg)
   {
      tell_player (p, " Format : tell <player> <msg>\n");
      return;
   }
   *msg++ = 0;
   p2 = find_player_name (str);
   if (!p2)
      return;
   if (p2 == p)
   {
      tell_player (p, " Cute.\n");
      return;
   }
   switch (asks (msg))
   {
   case '?':
      mid = "ask";
      break;
   case '!':
      mid = "exclaim";
      break;
   default:
      mid = "tell";
      break;
   }
   channel = CHAN_TELLS;
   tell_player (p2, "%s %ss you '%s'\n", p->name, mid, msg);
   channel = 0;
   tell_player (p, "You %s %s '%s'\n", mid, p2->name, msg);
   return;
}

void         remote (player * p, char *str)
{
   player      *p2;
   char        *msg, *mid = " ";

   msg = next_space (str);
   if (!*msg)
   {
      tell_player (p, " Format : remote <player> <msg>\n");
      return;
   }
   *msg++ = 0;
   p2 = find_player_name (str);
   if (!p2)
      return;
   if (p2 == p)
   {
      tell_player (p, " Cute.\n");
      return;
   }
   if (*msg == '\'' || *msg == ',')
      mid = "";
   channel = CHAN_TELLS;
   tell_player (p2, "%s%s%s\n", p->name, mid, msg);
   channel = 0;
   tell_player (p, " You emote '%s%s%s' to %s\n", p->name, mid, msg, p2->name);
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

void         c_who (player * p, char *str)
{
   char        *ref = stack;
   player      *scan;
   int          count;
   char         temp[20];

   for (scan = p_top, count = 0; scan; scan = scan->p_next)
      if (scan->flags & HERE && scan->room && scan->name)
	 count++;
   if (count == 1)
      strcpy (temp, "There is one user online");
   else
      sprintf (temp, "There are %d users online", count);
   banner (p, stack, temp, '-');
   stack = strchr (stack, 0);
   for (scan = p_top; scan; scan = scan->p_next)
      if (scan->room && scan->name)
      {
	 sprintf (stack, "%s %s\n", scan->name, scan->title);
	 stack = strchr (stack, 0);
	 if (p->residency & SU)
	 {
	    sprintf (stack, "   site: %s (%s:%d)\n", scan->sock.host,
		     scan->sock.ip, scan->sock.port);
	    stack = strchr (stack, 0);
	 }
      }
   banner (p, stack, 0, '-');
   stack = strchr (stack, 0);
   *stack++ = 0;
   writep (p, ref);
   stack = ref;
   return;
}

void         c_idle (player * p, char *str)
{
   player      *scan;
   char        *oldstack = stack;

   if (*str)
   {
      scan = find_player_name (str);
      if (!scan)
      {
	 writep (p, "I don't see that player here.\n");
	 return;
      }
      tell_player (p, "%s is %s idle.\n", scan->name, word_time (scan->idle));
      return;
   }
   banner (p, stack, "Idle", '-');
   stack = strchr (stack, 0);
   for (scan = p_top; scan; scan = scan->p_next)
      if (scan->name && scan->room)
      {
	 sprintf (stack, "%-25s : %s idle\n", scan->name, word_time (scan->idle));
	 stack = strchr (stack, 0);
      }
   banner (p, stack, 0, '-');
   stack = strchr (stack, 0);
   *stack++ = 0;
   writep (p, oldstack);
   stack = oldstack;
   return;
}

void         su_emote (player * p, char *str)
{
   char        *mid = " ";

   if (!*str)
   {
      writep (p, " Format : s;<emotion>\n");
      return;
   }
   if (*str == 39 || *str == 44)
      mid = "";
   channel = CHAN_SU;
   su_wall ("%s%s%s\n", p->name, mid, str);
   channel = 0;
   return;
}

void         su_think (player * p, char *str)
{
   if (!*str)
   {
      writep (p, " Format : s~<thought>\n");
      return;
   }
   channel = CHAN_SU;
   su_wall ("%s thinks . o O ( %s )\n", p->name, str);
   channel = 0;
   return;
}

void         su_say (player * p, char *str)
{
   char        *mid;

   if (!*str)
   {
      writep (p, "Format : s'<msg>\n");
      return;
   }
   switch (asks (str))
   {
   case '?':
      mid = "ask";
      break;
   case '!':
      mid = "exclaim";
      break;
   default:
      mid = "say";
   }
   channel = CHAN_SU;
   su_wall ("%s %ss \'%s\'\n", p->name, mid, str);
   channel = 0;
   return;
}
