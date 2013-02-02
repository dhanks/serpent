//////////////////////////////////////////////////////////////////
// File: buffer.c                                               //
//////////////////////////////////////////////////////////////////
// Copyright (c) 1998.  Serpent Development                     //
// Regents to Douglas Hanks (and slePP)                         //
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
#include <errno.h>

#include "config.h"
#include "flags.h"
#include "proto.h"
#include "structs.h"

#define INC_SIZE 256

void         init_buffer (player * p, const unsigned long int size)
{
   if (p->buffer.output)
      vlog ("buffer", "Player %s already has an init'd buffer?\n", p->name);
   p->buffer.output = (char *) malloc (size);
   p->buffer.size = p->buffer.isize = size;
   p->buffer.pos = 0;
   p->buffer.age = p->buffer.inc_age = 0;
   return;
}

void         deinit_buffer (player * p)
{
   if (p->buffer.output)
      free (p->buffer.output);
   p->buffer.output = NULL;
   p->buffer.size = 0;
   return;
}

int          buffer_add (player * p, char *data)
{
   int          len = strlen (data);
   unsigned long int t = (unsigned long int) time (NULL);

   if (!p->buffer.output)
      init_buffer (p, p->buffer.isize);

   while (p->buffer.size < (p->buffer.pos + len))
   {
      p->buffer.output = (char *) realloc (p->buffer.output, p->buffer.size + INC_SIZE);
      p->buffer.size += INC_SIZE;
   }

   memcpy (p->buffer.output + p->buffer.pos, data, len);
   p->buffer.pos += len;

   if (p->buffer.pos > p->buffer.isize)
      p->buffer.inc_age = t;
   p->buffer.age = t;
   return (len);
}

int          buffer_send (player * p)
{
   unsigned long int t = time (NULL);

   if (p->buffer.output && p->buffer.pos > 0)
   {
      p->buffer.output[p->buffer.pos] = '\0';
      if ((sock_send (&p->sock, p->buffer.output)) < 0)
      {

	 p->flags |= P_LOGOUT;
	 vlog ("buffer", "buffer_send: sock_send return -1 on %s\n", p->name);
	 return (-1);
      }
      p->buffer.pos = 0;
   }
   if (p->buffer.age > 0 && ((t - p->buffer.age) > 45))
   {
      deinit_buffer (p);
      p->buffer.age = 0;
      p->buffer.inc_age = 0;
   }

   if (p->buffer.inc_age > 0 && ((t - p->buffer.inc_age) > 15))
   {
      unsigned long int old_age = p->buffer.age;

      deinit_buffer (p);
      init_buffer (p, p->buffer.isize);
      p->buffer.inc_age = 0;
      p->buffer.age = old_age;
   }
   return (1);
}
