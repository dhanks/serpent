//////////////////////////////////////////////////////////////////
// File: set.c                                                  //
//////////////////////////////////////////////////////////////////
// Copyright (c) 1998.  Serpent Development                     //
// Regents to Douglas Hanks.                                    //
//////////////////////////////////////////////////////////////////
// You may reach Serpent Development via email at:              //
// Douglas Hanks: ima@realms.org                                //
// Tom Malcom   : tom@ackphft.matsu.alaska.edu                  //
//////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "flags.h"
#include "proto.h"
#include "structs.h"

void         c_password (player * p, char *str)
{
   if (!*str)
   {
      writep (p, "You cannot have a blank password, try again.\n");
      return;
   }
   strncpy (p->password, str, 12);
   tell_player (p, "Password has been set to: %s\n", p->password);
   save_player (p, 0);
   return;
}

void         c_title (player * p, char *str)
{
   if (!*str)
   {
      writep (p, "Usage: title <text>/reset\n");
      return;
   }
   if (!strcasecmp (str, "reset"))
   {
      p->title[0] = '\0';
      writep (p, "Your title has been reset.\n");
   }
   else
   {
      strncpy (p->title, str, P_TITLE_LEN);
      vwritep (p, "Your title has been set to...\n   %s %s\n", p->name, p->title);
   }
   return;
}


void         c_description (player * p, char *str)
{
   if (!*str)
   {
      writep (p, "Usage: description <text>/reset\n");
      return;
   }
   if (!strcasecmp (str, "reset"))
   {
      p->description[0] = '\0';
      writep (p, "Your description has been reset.\n");
   }
   else
   {
      strncpy (p->description, str, P_TITLE_LEN);
      vwritep (p, "Your description has been set to...\n%s\n", p->description);
   }
   return;
}


void         c_url (player * p, char *str)
{
   if (!*str)
   {
      writep (p, "Usage: url <your web site address>/reset\n");
      return;
   }
   if (!strcasecmp (str, "reset"))
   {
      p->url[0] = '\0';
      writep (p, "Your url has been reset.\n");
   }
   else
   {
      strncpy (p->url, str, P_TITLE_LEN);
      vwritep (p, "Your url has been set to %s\n", p->url);
   }
   return;
}


void         c_email (player * p, char *str)
{
   if (!*str)
   {
      writep (p, "Usage: email <your email address>\n");
      return;
   }
   if (!(strchr (str, '.') || strchr (str, '@')) || strchr (str, ' '))
   {
      writep (p, "Email addresses are usually of the form name@server.domain\n");
      return;
   }
   strncpy (p->email, str, P_TITLE_LEN);
   vwritep (p, "Your email has been set to %s\n", p->email);
   vlog ("emails", "%s : %s\n", p->name, p->email);
   save_player (p, 0);
   return;
}


void         c_prompt (player * p, char *str)
{
   if (!*str)
   {
      writep (p, "Usage: prompt <text>/reset\n");
      return;
   }
   if (!strcasecmp (str, "reset"))
   {
      p->prompt[0] = '\0';
      writep (p, "Your prompt has been reset.\n");
   }
   else
   {
      strncpy (p->prompt, str, P_TITLE_LEN);
      vwritep (p, "Your prompt has been set to: %s\n", p->prompt);
   }
   return;
}


void         c_entermsg (player * p, char *str)
{
   if (!*str)
   {
      writep (p, "Usage: entermsg <text>/reset\n");
      return;
   }
   if (!strcasecmp (str, "reset"))
   {
      p->entermsg[0] = '\0';
      writep (p, "Your entermsg has been reset.\n");
   }
   else
   {
      strncpy (p->entermsg, str, P_TITLE_LEN);
      vwritep (p, "Your entermsg has been set to...\n   %s %s\n", p->name, p->entermsg);
   }
   return;
}


void         c_conmsg (player * p, char *str)
{
   if (!*str)
   {
      writep (p, "Usage: conmsg <text>/reset\n");
      return;
   }
   if (!strcasecmp (str, "reset"))
   {
      p->conmsg[0] = '\0';
      writep (p, "Your conmsg has been reset.\n");
   }
   else
   {
      strncpy (p->conmsg, str, P_TITLE_LEN);
      vwritep (p, "Your conmsg has been set to...\n   %s %s\n", p->name, p->conmsg);
   }
   return;
}


void         c_disconmsg (player * p, char *str)
{
   if (!*str)
   {
      writep (p, "Usage: disconmsg <text>/reset\n");
      return;
   }
   if (!strcasecmp (str, "reset"))
   {
      p->disconmsg[0] = '\0';
      writep (p, "Your disconmsg has been reset.\n");
   }
   else
   {
      strncpy (p->disconmsg, str, P_TITLE_LEN);
      vwritep (p, "Your disconmsg has been set to...\n   %s %s\n", p->name, p->disconmsg);
   }
   return;
}


void         c_reconmsg (player * p, char *str)
{
   if (!*str)
   {
      writep (p, "Usage: reconmsg <text>/reset\n");
      return;
   }
   if (!strcasecmp (str, "reset"))
   {
      p->reconmsg[0] = '\0';
      writep (p, "Your reconmsg has been reset.\n");
   }
   else
   {
      strncpy (p->reconmsg, str, P_TITLE_LEN);
      vwritep (p, "Your reconmsg has been set to...\n   %s %s\n", p->name, p->reconmsg);
   }
   return;
}


void         c_recap (player * p, char *str)
{
   if (strcasecmp (p->name, str))
      vwritep (p, "Your name's %s, not %s!\n", p->name, str);
   else
   {
      strncpy (p->name, str, P_NAME_LEN);
      vwritep (p, "You recapitalise your name to %s\n", p->name);
   }
   return;
}

void         c_terminal (player * p, char *str)
{
   int          i = 0;

   if (!*str)
   {
      writep (p, "Usage: terminal <type>\n"
	      "                 type\n");
      return;
   }
   if (strcasecmp (str, "types"))
      for (i = 0; term_codes[i].key; i++)
	 if (!strcasecmp (term_codes[i].key, str))
	 {
	    p->termtype = i;

	    vwritep (p, "Terminal type set as %s.\n",
		     term_codes[i].key);

	    return;
	 }
   writep (p, termtypes_msg);
   return;
}
