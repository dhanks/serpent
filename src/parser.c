//////////////////////////////////////////////////////////////////
// file: parser.c                                               //
//       Command handler.                                       //
//////////////////////////////////////////////////////////////////
// Copyright (c) 1998.  Serpent Development                     //
// Regents to Douglas Hanks.                                    //
//////////////////////////////////////////////////////////////////
// You may reach Serpent Development via email at:              //
// Douglas Hanks: ima@realms.org                                //
// Tom Malcom   : tom@ackphft.matsu.alaska.edu                  //
//////////////////////////////////////////////////////////////////

#include <ctype.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "commands.h"
#include "config.h"
#include "flags.h"
#include "proto.h"
#include "structs.h"

command     *root_commands[27];

int          init_command_modules (void)
{
   command     *com;
   int          i;
   char         location[80];

   if (command_handle != NULL)
      dlclose (command_handle);
   vlog ("boot", " -=> Initiating dynamic ELF module commands.\n");
   sprintf(location, "%smodules/modules/commands.so", SERVER_ROOT);
   if ((command_handle = dlopen(location, RTLD_LAZY)) == NULL)     
    {
      vlog ("modules", " -=> dlopen received NULL: %s\n", dlerror());
      return (-EINVAL);
    }

  for (i = 0; i < 27; i++)
   {
      for (com = root_commands[i]; com->key; com++)
      {
         if (com->dyn == YES)
         {
            
            if ((com->comm_func = (void (*)(player *, char *))
          dlsym(command_handle, com->func_name)) == NULL)            {
               com->comm_func = NULL;
               vlog ("modules", " ** -=> Command '%s' doesn't have a function: %s\n", com->key, dlerror());
            }
         }
      }
   }
   return 1;
}



void         init_parser (void)
{
   command     *com;
   int          i;

   for (com = main_command_list, i = 0; i < 27; i++)
   {
      for (root_commands[i] = com; com->key; com++);
      com++;
   }
   return;
}


void         view_subcommands (player * p, command * parent)
{
   command     *com, *top;
   char        *ref = stack;

   com = parent->sublist;
   sprintf (stack, "Available %s subcommands...\n  ", parent->key);
   for (top = com++; com->key; com++)
      if (com->level <= p->residency)
      {
	 stack = strchr (stack, 0);
	 sprintf (stack, "%s  ", com->key);
      }
   if (stack == ref)
      vwritep (p, "No %s subcommands available to you.\n", parent->key);
   else
   {
      stack = strchr (stack, 0);
      *(stack - 2) = '\n';
      *(stack - 1) = '\0';
      writep (p, ref);
      stack = ref;
   }
   return;
}


void         c_commands (player * p, char *str)
{
   int          i, len = 0, count;
   command     *com;
   char        *ref = stack;
   char         temp[80];

   for (i = 0, count = 0; i < 27; i++)
      for (com = root_commands[i]; com->key; com++)
	 if (com->level <= p->residency)
	    count++;
   sprintf (temp, "Your command set contains %d commands", count);
   banner (p, stack, temp, '-');
   stack = strchr (stack, 0);
   *stack++ = ' ';
   *stack++ = ' ';
   for (i = 0; i < 27; i++)
      for (com = root_commands[i]; com->key; com++)
	 if (com->level <= p->residency)
	 {
	    len += (strlen (com->key) + 2);
	    if (len < 77)
	    {
	       sprintf (stack, "%s, ", com->key);
	       stack = strchr (stack, 0);
	    }
	    else
	    {
	       len = 0;
	       *stack++ = '\n';
	       *stack++ = ' ';
	       *stack++ = ' ';
	       len += (strlen (com->key) + 2);
	       sprintf (stack, "%s, ", com->key);
	       stack = strchr (stack, 0);
	    }
	 }
   stack--;
   stack--;
   *stack++ = '.';
   *stack++ = '\n';
   banner (p, stack, 0, '-');
   stack = strchr (stack, 0);
   *stack++ = 0;
   writep (p, ref);
   stack = ref;
   return;
}



void         c_aliases (player * p, char *str)
{
   int          i;
   command     *com;
   char        *ref = stack;

   strcpy (stack, "Available command aliases:\n  ");
   for (i = 0; i < 27; i++)
      for (com = root_commands[i]; com->key; com++)
	 if (com->level <= p->residency && com->flags & c_ALS)
	 {
	    stack = strchr (stack, 0);
	    sprintf (stack, "%s  ", com->key);
	 }
   if (stack == ref)
      writep (p, "No command aliases available to you.\n");
   else
   {
      *(stack - 2) = '\n';
      *(stack - 1) = '\0';
      writep (p, ref);
      stack = ref;
   }
   return;
}


void         c_null (player * p, char *str)
{
   writep (p, "Erk. This is the subcommand safety net. There's something "
	   "wrong with that command.\n");
   log ("parser", "c_null: Arrived in c_null - check the command list.\n");
   return;
}


void         actually_do_command (player * p, command * com, char *str)
{
#if defined(STACK_DEBUG)
   char        *sanity = stack;

#endif
   current_func = com->key;
   current_player = p;
   if (!com->comm_func)
      c_null (p, 0);
   else
      (com->comm_func) (p, str);
#if defined(STACK_DEBUG)
   if (stack != sanity)
   {
      sprintf (sanity, "Reclaimed %d bytes from command %s\n", (int) stack - (int) sanity, com->key);
      log ("stack", sanity);
      stack = sanity;
   }
#endif
   current_func = 0;
   current_player = 0;
   p->commands_out++;
   p->idle = 0;
   return;
}


command     *find_subcommand (player * p, char *key, command * parent)
{
   command     *com;

   trim_end (key);
   for (com = parent->sublist; com->key; com++)
      if (com->level <= p->residency && !strncasecmp (key, com->key, strlen (key)))
	 return com;
   return NULL;
}


void         run_subcommand (player * p, char *str, command * parent)
{
   command     *com;
   char        *param;

   trim_end (str);
   if (!parent->sublist)
   {
      writep (p, "No subcommands found!\n");
      vlog ("parser", "run_subcommand: No %s subcommands found!\n", parent->key);
      return;
   }
   if (!*str)
   {
      view_subcommands (p, parent);
      return;
   }
   param = next_space (str);
   if (*param)
      *param++ = '\0';
   com = find_subcommand (p, str, parent);
   if (com == NULL)
   {
      vwritep (p, "No command %s %s available to you.\n", parent->key, str);
      return;
   }
   else
      actually_do_command (p, com, param);
   return;
}


char        *match_command_key (command * com, char *str)
{
   char        *key = com->key;

   for (; *key; key++, str++)
      if (tolower ((int) *key) != tolower ((int) *str))
	 return '\0';
   if (!(com->flags & c_NSP) && *str && *str != ' ')
      return 0;
   for (; *str && *str == ' '; str++);
   return str;
}


void         run_command (player * p, char *str)
{
   command     *com;
   char        *param;

   for (; *str == ' '; str++);
   trim_end (str);
   if (!*str)
      return;
   if (!isalpha (*str))
      com = root_commands[0];
   else
      com = root_commands[((tolower (*str) - 'a') + 1)];
   for (; com->key; com++)
      if ((param = match_command_key (com, str)) && com->level <= p->residency)
      {

	 if (com->sublist)
	    run_subcommand (p, param, com);
	 else
	    actually_do_command (p, com, param);
	 return;
      }
   param = next_space (str);
   if (*param)
      *param++ = '\0';
   vwritep (p, "No command %s available to you.\n", str);
   return;
}
