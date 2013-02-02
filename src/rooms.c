//////////////////////////////////////////////////////////////////
// File: rooms.c                                                //
//       Contains functions to create, link, unlink, destroy,   //
//       retrieve, save, and search rooms.                      //
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
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>

#include "config.h"
#include "flags.h"
#include "proto.h"
#include "structs.h"

room        *public_hash_table[10] =
{0, 0, 0, 0, 0, 0, 0, 0, 0};

int          room_index[5000];	// This is our global room index.                

struct s_pub_room public;

void         command_exits (player *, char *);
void         command_look (player *, char *);

room        *malloc_room (void)
{
   room        *r = (room *) malloc (sizeof (room));

   if (!r)
   {
      log ("malloc", "malloc room: malloc failed.");
      return 0;
   }

   memset (r, '\0', sizeof (room));
   r->id = new_rid ();
   return r;
}


///////////////////////////////////////////////////////////////////////////////
//                        Public room modules only                           //
///////////////////////////////////////////////////////////////////////////////

room        *find_coordinates (int x, int y, int z)
{
   room        *scan;
   int          i;


   for (i = 0; i < 10; i++)
      for (scan = public_hash_table[i]; scan; scan = scan->next)
	 if (scan->x == x && scan->y == y && scan->z == z)
	    return scan;
   return 0;
}

room        *find_public_room (int id)
{
   room        *scan;
   int          i;

   for (i = 0; i < 10; i++)
      for (scan = public_hash_table[i]; scan; scan = scan->next)
	 if (scan->id == id)
	    return scan;
   return 0;
}

void         link_public_room (room * r)
{
   r->next = public_hash_table[(r->id % 10)];
   if (r->next)
      r->next->prev = r;
   public_hash_table[(r->id % 10)] = r;
   return;
}

void         unlink_public_room (room * r)
{
   if (r->next)
      r->next->prev = r->prev;
   if (r->prev)
      r->prev->next = r->next;
   else
      public_hash_table[(r->id % 10)] = r->next;
   return;
}

void         destroy_public_room (room * r)
{
   if (!r)
      return;
   unlink_public_room (r);
   free (r);
   return;
}

///////////////////////////////////////////////////////////////////////////////
//                           General Room Modules                            //
///////////////////////////////////////////////////////////////////////////////



void         draw_map (player * p, room * r)
{
   if (r->n)
      tell_player (p, "-      N      -\n");
   else
      tell_player (p, "-      -      -\n");

   if (r->e)
      tell_player (p, "E <-");
   else
      tell_player (p, "- <-");

   if (r->u)
      tell_player (p, "U-(M)-");
   else
      tell_player (p, "--(M)-");

   if (r->d)
      tell_player (p, "D-> ");
   else
      tell_player (p, "--> ");

   if (r->w)
      tell_player (p, "W\n");
   else
      tell_player (p, "-\n");

   if (r->s)
      tell_player (p, "-      S      -\n");
   else
      tell_player (p, "-      -      -\n");
}

int          new_rid (void)
{
   int          i;

   for (i = 1; i <= 10000 && find_public_room (i); i++);
   return i;
}

int          link_to_room (player * p, room * r)
{
   if (!r)
      return -1;
   if (!p)
      return -1;

   p->room = r;
   p->room_next = r->top_player;
   p->room_prev = 0;
   if (r->top_player)
      r->top_player->room_prev = p;
   r->top_player = p;
   return 0;
}

void         unlink_from_room (player * p)
{
   if (!p->room)
      return;

   if (p->room->top_player == p)
      p->room->top_player = p->room_next;
   else
      p->room_prev->room_next = p->room_next;
   if (p->room_next)
      p->room_next->room_prev = p->room_prev;
   p->room_next = 0;
   p->room_prev = 0;
   return;
}

int          move_to (player * p, room * r)
{
   int          i;
   room        *last;

   if (!r)
   {
      log ("error", "move to: no room!");
      return -1;
   }

   last = p->room;
   if (p->flags & HERE)
   {
      vwriter (p, 0, last, "%s %s\n", p->name, r->entermsg);
      vwriter (p, 0, r, "%s %s\n", p->name, p->entermsg);
   }
   unlink_from_room (p);
   i = link_to_room (p, r);
   return i;
}


// Sorry, I'm a hardcore MUD fan :D 

void         make_exits (room * r)
{
   r->n = find_coordinates (r->x, r->y + 1, r->z);
   r->e = find_coordinates (r->x + 1, r->y, r->z);
   r->s = find_coordinates (r->x, r->y - 1, r->z);
   r->w = find_coordinates (r->x - 1, r->y, r->z);
   r->u = find_coordinates (r->x, r->y, r->z + 1);
   r->d = find_coordinates (r->x, r->y, r->z - 1);
   return;
}

void         make_public_exits (void)
{
   room        *scan;
   int          i;

   for (i = 0; i < 10; i++)
      for (scan = public_hash_table[i]; scan; scan = scan->next)
      {
	 make_exits (scan);

      }
   return;
}

//////////////////////////////////////////////////////////////////////////////
//                                 I/O                                      //
//////////////////////////////////////////////////////////////////////////////

void         get_r_flags (char *str)
{

   if (!*str)
      return;
   else if (!strcasecmp (str, "MAIN"))
      public.flags |= R_MAIN;
   else if (!strcasecmp (str, "VOID"))
      public.flags |= R_VOID;
   else if (!strcasecmp (str, "block_north"))
      public.flags |= R_BLOCK_N;
   else if (!strcasecmp (str, "block_east"))
      public.flags |= R_BLOCK_E;
   else if (!strcasecmp (str, "block_west"))
      public.flags |= R_BLOCK_W;
   else if (!strcasecmp (str, "block_south"))
      public.flags |= R_BLOCK_S;
   else
      public.flags |= R_NOFLAGS;
}

void         get_room_keys (char *sec, char *param)
{
   if (!strcasecmp (sec, "name"))
      strcpy (public.name, param);
   else if (!strcasecmp (sec, "entermsg"))
      strcpy (public.entermsg, param);
   else if (!strcasecmp (sec, "x"))
      public.x = atoi (param);
   else if (!strcasecmp (sec, "y"))
      public.y = atoi (param);
   else if (!strcasecmp (sec, "z"))
      public.z = atoi (param);
   else if (!strcasecmp (sec, "flags"))
      get_r_flags (param);


}

char        *find_match (char *pri, char *dat, int *size)
{
   char        *sec, *param;

   param = next_space (pri);

   if (*param)
      *param++ = 0;

   pri = trim_end (pri);
   sec = strchr (pri, '.');

   if (sec)
      *sec++ = 0;
   else
   {
      vlog ("boot", " -=> find_match(room): no class supplied.\n");
      return dat;
   }

   if (!strcasecmp (pri, "room"))
   {
      get_room_keys (sec, param);

   }
   return dat;
}

void         open_public_room (char *file)
{
   dfile        f;
   char        *data, *ref;
   room        *r;
   char         temp[80];

   r = malloc_room ();

   sprintf (temp, "%slib/rooms/public/%s.pub", SERVER_ROOT, file);
   f = load_dfile (temp);
   sprintf (temp, "%slib/rooms/public/%s.desc", SERVER_ROOT, file);
   r->desc = load_dfile (temp);

   if (!f.data)
   {
      vlog ("error", "No data .. hmm\n");
      return;
   }

   if (!r->desc.data)
      vlog ("error", "No data in %s.desc\n", file);


   data = f.data;


   while (f.size)
   {
      ref = stack;
      data = get_line (data, &f.size);
      if (*ref != '#')
	 data = find_match (ref, data, &f.size);
      stack = ref;
   }

   r->x = public.x;
   r->y = public.y;
   r->z = public.z;
   r->flags = public.flags;
   public.flags = 0;
   strcpy (r->entermsg, public.entermsg);
   strcpy (r->name, public.name);
   link_public_room (r);
   free (f.name);
   free (f.data);
   return;
}


void         load_public_rooms (void)
{
   char           *ext;
   DIR           *file_dir;
   struct dirent *file_name;
   char           location[80];

   sprintf(location, "%slib/rooms/public", SERVER_ROOT);
   file_dir = opendir (location);
   vlog ("boot", " -=> Loading system rooms.\n");

   for (file_name = readdir (file_dir);
	file_name != NULL;
	file_name = readdir (file_dir))
   {
      ext = strrchr (file_name->d_name, '.');
      if (!ext)
	 continue;
      if (!strcasecmp (ext, ".pub"))
      {
	 *ext = 0;
	 open_public_room (file_name->d_name);
      }
   }
   if (file_dir)
      closedir (file_dir);
   // And relax :D                                            
}

void         init_public_rooms (void)
{
   room        *scan;
   int          i;

   load_public_rooms ();

   for (i = 0; i < 10; i++)
   {
      for (scan = public_hash_table[i]; scan; scan = scan->next)
      {
	 if (scan->flags & R_MAIN)
	    mainroom = scan;
	 if (scan->flags & R_VOID)
	    voidroom = scan;
      }
   }

   make_public_exits ();


   if (!mainroom)
      log ("boot", " -=> Failed to find main room?\n");
   if (!voidroom)
      log ("boot", " -=> Failed to find void room?\n");
   return;
}

//////////////////////////////////////////////////////////////////////////////
//                             Room commands                                //
//////////////////////////////////////////////////////////////////////////////

void         go_north (player * p, char *str)
{
   room        *r;

   r = find_coordinates (p->room->x, p->room->y + 1, p->room->z);

   if (p->room->flags & R_BLOCK_N)
   {
      tell_player (p, "You can't go there.  It is blocked off.\n");
      return;
   }
   if (!r)
   {
      tell_player (p, "Nothing exists there.\n");
      return;
   }
   if (move_to (p, r))
   {
      tell_player (p, "Hmm. An error occured somewhere.\n");

      return;
   }
   command_look (p, 0);
   return;
}


void         go_east (player * p, char *str)
{
   room        *r;

   r = find_coordinates (p->room->x + 1, p->room->y, p->room->z);

   if (p->room->flags & R_BLOCK_E)
   {
      tell_player (p, "You can't go there.  It is blocked off.\n");
      return;
   }
   if (!r)
   {
      tell_player (p, "Nothing exists there.\n");
      return;
   }
   if (move_to (p, r))
   {
      tell_player (p, "Hmm. An error occured somewhere.\n");

      return;
   }
   command_look (p, 0);
   return;
}


void         go_south (player * p, char *str)
{
   room        *r;

   r = find_coordinates (p->room->x, p->room->y - 1, p->room->z);

   if (p->room->flags & R_BLOCK_S)
   {
      tell_player (p, "You can't go there.  It is blocked off.\n");
      return;
   }
   if (!r)
   {
      tell_player (p, "Nothing exists there.\n");
      return;
   }
   if (move_to (p, r))
   {
      tell_player (p, "Hmm. An error occured somewhere.\n");

      return;
   }
   command_look (p, 0);
   return;
}


void         go_west (player * p, char *str)
{
   room        *r;

   r = find_coordinates (p->room->x - 1, p->room->y, p->room->z);

   if (p->room->flags & R_BLOCK_W)
   {
      tell_player (p, "You can't go there.  It is blocked off.\n");
      return;
   }
   if (!r)
   {
      tell_player (p, "Nothing exists there.\n");
      return;
   }
   if (move_to (p, r))
   {
      tell_player (p, "Hmm. An error occured somewhere.\n");

      return;
   }
   command_look (p, 0);
   return;
}


void         go_up (player * p, char *str)
{
   room        *r;

   r = find_coordinates (p->room->x, p->room->y, p->room->z + 1);


   if (p->room->flags & R_BLOCK_U)
   {
      tell_player (p, "You can't go there.  It is blocked off.\n");
      return;
   }
   if (!r)
   {
      tell_player (p, "Nothing exists there.\n");
      return;
   }
   if (move_to (p, r))
   {
      tell_player (p, "Hmm. An error occured somewhere.\n");

      return;
   }
   command_look (p, 0);
   return;
}


void         go_down (player * p, char *str)
{
   room        *r;

   r = find_coordinates (p->room->x, p->room->y, p->room->z - 1);

   if (p->room->flags & R_BLOCK_D)
   {
      tell_player (p, "You can't go there.  It is blocked off.\n");
      return;
   }
   if (!r)
   {
      tell_player (p, "Nothing exists there.\n");
      return;
   }
   if (move_to (p, r))
   {
      tell_player (p, "Hmm. An error occured somewhere.\n");

      return;
   }
   command_look (p, 0);
   return;
}

void         command_look (player * p, char *str)
{
   char        *ref = stack;
   player      *scan;

   if (!p->room)
   {
      move_to (p, voidroom);
      if (p->room)
	 command_look (p, 0);
      else
	 writep (p, " Cannot find you a room!\n");
      return;
   }
   if (p->flags & SHOW_MAPS)
      draw_map (p, p->room);
   sprintf (stack, "[Location: %s][%d,%d,%d]\n%s", p->room->name,
	    p->room->x, p->room->y, p->room->z,
	    p->room->desc.data);
   stack = strchr (stack, 0);

   for (scan = p_top; scan; scan = scan->p_next)
   {

      if (scan != p && scan->room == p->room && scan->flags & HERE)
      {
	 sprintf (stack, "%s %s\n", scan->name, scan->title);
	 stack = strchr (stack, 0);
      }
   }

   *stack++ = 0;
   writep (p, ref);
   stack = ref;
   if (p->flags & SHOW_EXITS)
      command_exits (p, 0);
   return;
}




void         command_jump (player * p, char *str)
{
   room        *r;
   char        *y, *z;

   if (!*str)
   {
      writep (p, " Format : jump <x y z>\n");
      return;
   }

   y = next_space (str);
   z = next_space (y);

   if (!*y || !*z)
   {
      writep (p, " Format : jump <x y z>\n");
      return;
   }
   tell_player (p, "%d %d %d\n", atoi (str), atoi (y), atoi (z));
   r = find_coordinates (atoi (str), atoi (y), atoi (z));
   if (!r)
   {
      writep (p, " Couldn't find that location.\n");
      return;
   }

   move_to (p, r);
   command_look (p, 0);
   return;
}








void         command_rooms (player * p, char *str)
{
   room        *r;
   int          public_rooms = 0, x;
   char        *ref = stack;

   strcpy (stack, "Public rooms\n");
   stack = strchr (stack, 0);
   for (x = 0; x < 10; x++)
      for (r = public_hash_table[x]; r; r = r->next)
      {
	 public_rooms++;

	 sprintf (stack, "Room (rid %03d) [%s] (%d,%d,%d)\n", r->id, r->name, r->x, r->y, r->z);
	 stack = strchr (stack, 0);
      }

   *stack++ = 0;
   writep (p, ref);
   stack = ref;
   return;
}

void         command_exits (player * p, char *str)
{
   char        *ref = stack;
   int          i = 0;

   if (!p->room)
   {
      writep (p, " That's strange! You're nowhere!\n");
      return;
   }

   strcpy (stack, "Obvious exits:\n");
   stack = strchr (stack, 0);

   if (p->room->n)
   {
      if (p->room->flags & R_BLOCK_N)
	 sprintf (stack, " North: %s (blocked)\n", p->room->n->name);
      else
	 sprintf (stack, " North:  %s\n", p->room->n->name);
      stack = strchr (stack, 0);
      i = 1;
   }
   if (p->room->e)
   {
      if (p->room->flags & R_BLOCK_E)
	 sprintf (stack, " East:  %s (blocked)\n", p->room->e->name);
      else
	 sprintf (stack, " East:   %s\n", p->room->e->name);
      stack = strchr (stack, 0);
      i = 1;
   }
   if (p->room->s)
   {
      if (p->room->flags & R_BLOCK_S)
	 sprintf (stack, " South: %s (blocked)\n", p->room->s->name);
      else
	 sprintf (stack, " South:  %s\n", p->room->s->name);
      stack = strchr (stack, 0);
      i = 1;
   }
   if (p->room->w)
   {
      if (p->room->flags & R_BLOCK_W)
	 sprintf (stack, " West:  %s (blocked)\n", p->room->w->name);
      else
	 sprintf (stack, " West:   %s\n", p->room->w->name);
      stack = strchr (stack, 0);
      i = 1;
   }
   if (p->room->u)
   {
      if (p->room->flags & R_BLOCK_U)
	 sprintf (stack, " Up:     %s (blocked)\n", p->room->u->name);
      else
	 sprintf (stack, " Up:     %s\n", p->room->u->name);
      stack = strchr (stack, 0);
      i = 1;
   }
   if (p->room->d)
   {
      if (p->room->flags & R_BLOCK_D)
	 sprintf (stack, " Down:  %s (blocked)\n", p->room->d->name);
      else
	 sprintf (stack, " Down:   %s\n", p->room->d->name);
      stack = strchr (stack, 0);
      i = 1;
   }

   if (i)
   {
      *stack++ = 0;
      writep (p, ref);
   }
   else
      writep (p, "There are no exits from here.\n");
   stack = ref;
   return;
}

// I don't know about you, but I'm breathing pretty heavily                  //
// let's relax                                                               //
