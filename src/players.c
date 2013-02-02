//////////////////////////////////////////////////////////////////
// File: players.c                                              //
//////////////////////////////////////////////////////////////////
// Copyright (c) 1998.  Serpent Development                     //
// Regents to Douglas Hanks.                                    //
//////////////////////////////////////////////////////////////////
// You may reach Serpent Development via email at:              //
// Douglas Hanks: ima@realms.org                                //
// Tom Malcom   : tom@ackphft.matsu.alaska.edu                  //
//////////////////////////////////////////////////////////////////


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "flags.h"
#include "proto.h"
#include "structs.h"


player      *malloc_player (void)
{
   player      *p = (player *) malloc (sizeof (player));

   if (!p)
      fatal ("malloc_player: Out of memory!\n");
   else
      memset (p, 0, sizeof (player));
   purge_player (p);
   setup_player (p);
   return p;
}


void         free_player (player * p)
{
   free (p);
   return;
}


void         link_player (player * p)
{
   p->p_next = p_top;
   if (p_top)
      p_top->p_prev = p;
   p_top = p;
   return;
}


void         unlink_player (player * p)
{
   if (p->p_next)
      p->p_next->p_prev = p->p_prev;
   if (p->p_prev)
      p->p_prev->p_next = p->p_next;
   else
      p_top = p->p_next;
   p->p_next = 0;
   p->p_prev = 0;
   return;
}


player      *find_player_id (int id)
{
   player      *p;

   for (p = p_top; p; p = p->p_next)
      if (p->id == id)
	 return p;
   return NULL;
}


player      *find_player_name (char *name)
{
   player      *p;

   for (p = p_top; p; p = p->p_next)
      if (!strncasecmp (name, p->name, strlen (name)))
	 return p;
   return NULL;
}


int          match_name (char *name, char *str)
{
   for (; *str; name++, str++)
      if (!*name || tolower ((int) *name) != tolower ((int) *str))
	 return 0;
   return 1;
}

int          new_pid (void)
{
   int          i;

   for (i = 1; find_player_id (i); i++);
   if (i > P_MAX_ID || i < 1)
      fatal ("new_pid: Ran out of player ids!\n");
   return i;
}


void         purge_player (player * p)
{
   init_buffer (p, BUF_SIZE);
   p->last_seen = 0;
   p->name[0] = '\0';
   p->id = new_pid ();
   p->password[0] = '\0';
   p->password_fails = 0;
   p->termx = DEF_TERMX;
   p->residency = BASE;
   p->gender = NEUTER;
   p->room = NULL;
   p->title[0] = '\0';
   p->description[0] = '\0';
   p->url[0] = '\0';
   p->email[0] = '\0';
   strcpy (p->prompt, "-> ");
   p->entermsg[0] = '\0';
   p->conmsg[0] = '\0';
   p->disconmsg[0] = '\0';
   p->reconmsg[0] = '\0';
   p->login_time = 0;
   p->on_since = 0;
   return;
}


void         setup_player (player * p)
{
   strncpy (p->title, "is uninitiated in the way of the realm.", P_TITLE_LEN);
   strncpy (p->description, "not set", P_DESCRIPTION_LEN);
   strncpy (p->entermsg, "enters in a standard kind of way.", P_ENTERMSG_LEN);
   strncpy (p->conmsg, "hath entered the realm.", P_CONMSG_LEN);
   strncpy (p->disconmsg, "hath left the realm.", P_DISCONMSG_LEN);
   strncpy (p->reconmsg, "reconnects.", P_RECONMSG_LEN);
   return;
}
