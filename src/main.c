//////////////////////////////////////////////////////////////////
// File: main.c                                                 //
//       Where it all begins                                    //
//////////////////////////////////////////////////////////////////
// Copyright (c) 1998.  Serpent Development                     //
// Regents to Douglas Hanks.                                    //
//////////////////////////////////////////////////////////////////
// You may reach Serpent Development via email at:              //
// Douglas Hanks: ima@realms.org                                //
// Tom Malcom   : tom@ackphft.matsu.alaska.edu                  //
//////////////////////////////////////////////////////////////////

#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <unistd.h>

#include "config.h"
#include "flags.h"
#include "proto.h"
#include "structs.h"

#if DUALPLEX
struct sock  sock_lis[2];

#else
struct sock  sock_lis;

#endif

void         destroy_player (player * scan)
{
   channel = CHAN_SYSTEM;
   if (!(scan->flags & P_RECON))
      if (scan->flags & HERE)
      {
#if DUALPLEX
	 if (scan->dual == 1)
	    vwriter (scan, 0, scan->room, "[%s %s][dualplex two]\n", scan->name,
		     scan->disconmsg);
	 else
	    vwriter (scan, 0, scan->room, "[%s %s][dualplex one]\n", scan->name,
		     scan->disconmsg);
#else
	 vwriter (scan, 0, scan->room, "[%s %s]\n", scan->name,
		  scan->disconmsg);
#endif
      }
   channel = 0;
   scan->last_seen = time (0);
   scan->flags &= ~HERE;
   if (scan->residency & RESIDENT)
      save_player (scan, 0);
   deinit_buffer (scan);
   unlink_player (scan);
   shutdown (scan->sock.fd, 2);
   close (scan->sock.fd);
   scan->sock.fd = -1;
   free_player (scan);
   s_info.connections--;
}

void         shutdown_server (void)
{
   player      *p;

#if DUALPLEX
   int          i;

#endif
   for (p = p_top; p; p = p->p_next)
   {
      writep (p, "\n\nServer shutting down now.\n\n");

      destroy_player (p);
   }
#if DUALPLEX
   for (i = 0; i < 2; i++)
   {
      shutdown (sock_lis[i].fd, 2);
      close (sock_lis[i].fd);
      sock_lis[i].fd = -1;
   }
#else
   shutdown (sock_lis.fd, 2);
   close (sock_lis.fd);
   sock_lis.fd = -1;
#endif
   stop_timer ();
   free (stack);
   free (termtypes_msg);
   free (connect_msg.data);
   free (motd_msg.data);
   free (dis_msg.data);
   free (screen_one_msg.data);
   free (terminals_msg.data);
   free (compiled_msg.data);

   log ("system", "Server closed down.\n");
   return;
}


void         abort_boot (char *str)
{
   fprintf (stderr, "%s\nBoot aborted.\n", str);
   exit (-1);
}


void         fatal (char *str)
{
   FILE        *f;
   char        *ref = stack;
   char         location[80];
   
   log ("fatal", str);
   writea (str);
   sprintf(location, "%slogs/dump", SERVER_ROOT);
   f = fopen (location, "a");
   
   if (!f)
   {
      puts ("Unable to open dump file!\n");
      system_flags |= PATH_ERROR;
   }
   else
   {
      sprintf (stack,
	       "===== Begin crash dump =====\n"
	       "timestamp      : %s\n"
	       "fatal log      : %s"
	       "errno          : %s (%d)\n"
	       "config.path    : %s\n"
	       "config.flags   : %d\n",
	       logtime (), str, sys_errlist[errno], errno,
	       CONFIG_PATH, system_flags);
      stack = strchr (stack, 0);
      if (current_func)
      {
	 sprintf (stack,
		  "last command   : %s\n", current_func);
	 stack = strchr (stack, 0);
      }
      if (current_player)
      {
	 sprintf (stack,
		  "current player : %s\n"
		  "c player flags : %d\n", current_player->name,
		  current_player->flags);
	 stack = strchr (stack, 0);
      }
      strcpy (stack,
	      "===== End crash dump =====\n");
      stack = end_string (ref);
      fprintf (f, ref);

   }
   stack = ref;
   fclose (f);
   shutdown_server ();
   exit (-1);
   return;
}

void         init_dfiles (void)
{
   char location[80];
   
   vlog ("boot", " -=> Loading system files.\n");
   free (connect_msg.data);
   sprintf(location, "%slib/files/connect", SERVER_ROOT);
   connect_msg = load_dfile (location);
   free (motd_msg.data);
   sprintf(location, "%slib/files/motd", SERVER_ROOT);
   motd_msg = load_dfile (location);
   free (dis_msg.data);
   sprintf(location, "%slib/files/dis", SERVER_ROOT);
   dis_msg = load_dfile (location);
   free (screen_one_msg.data);
   sprintf(location, "%slib/files/welcome", SERVER_ROOT);
   screen_one_msg = load_dfile (location);
   free (terminals_msg.data);
   sprintf(location, "%slib/files/terms", SERVER_ROOT);
   terminals_msg = load_dfile (location);
   free (compiled_msg.data);
   sprintf(location, "%slib/files/compiled", SERVER_ROOT);
   compiled_msg = load_dfile (location);
   return;
}

void         player_disconnect (player * p)
{
   shutdown (p->sock.fd, 2);
   close (p->sock.fd);
}

void         sync_pid (int pid)
{
   FILE        *out;
   char         location[80];
     
   sprintf(location, "%stmp/PID", SERVER_ROOT);
   out = fopen (location, "w");

   if (!out)
   {
      vlog ("pid", "Couldn't write PID!\n");
      return;
   }
   fprintf (out, "%d", pid);
   fclose (out);
}

void         core_loop (void)
{
#if DUALPLEX
   fd_set       fds_dual_one, fds_dual_two;
   int          i, r[2], largest[2];
   struct timeval tv[2];

#else
   fd_set       fds;
   int          r, largest;
   struct timeval tv;

#endif
   struct s_player *scan;
   time_t       current_time;

#if DUALPLEX
   DUAL_ONE_FD_ZERO (&fds_dual_one);
   DUAL_ONE_FD_SET (sock_lis[0].fd);
   DUAL_TWO_FD_ZERO (&fds_dual_two);
   DUAL_TWO_FD_SET (sock_lis[1].fd);
#else
   S_FD_ZERO (&fds);
   S_FD_SET (sock_lis.fd);
#endif
   current_time = time (0);
   strftime (s_info.time, 49, "%A %d %B %Y", localtime (&current_time));

   if (p_top)
      for (scan = p_top; scan; scan = scan->p_next)
	 if (scan->sock.fd < 0)
	    destroy_player (scan);
	 else
	 {
#if DUALPLEX
	    if (scan->dual == 1)
	    {
	       DUAL_TWO_FD_SET (scan->sock.fd);
	    }
	    else
	    {
	       DUAL_ONE_FD_SET (scan->sock.fd);
	    }
#else
	    S_FD_SET (scan->sock.fd);
#endif
	 }
#if DUALPLEX
   for (i = 0; i < 2; i++)
   {
      tv[i].tv_sec = 300;
      tv[i].tv_usec = 0;
      r[i] = 0;
   }

   r[0] = select (largest[0] + 1, &fds_dual_one, NULL, NULL, &tv[0]);
   if (r[0] > 0)
   {
      if (p_top)
	 for (scan = p_top; scan; scan = scan->p_next)
	    if (DUAL_ONE_FD_ISSET (scan->sock.fd))
	       player_input (scan);
      if (DUAL_ONE_FD_ISSET (sock_lis[0].fd))
      {
	 accept_connection (0);
      }
   }
   r[1] = select (largest[1] + 1, &fds_dual_two, NULL, NULL, &tv[1]);
   if (r[1] > 0)
   {
      if (p_top)
	 for (scan = p_top; scan; scan = scan->p_next)
	    if (DUAL_TWO_FD_ISSET (scan->sock.fd))
	       player_input (scan);
      if (DUAL_TWO_FD_ISSET (sock_lis[1].fd))
      {
	 accept_connection (1);
      }
   }
#else
   tv.tv_sec = 300;
   tv.tv_usec = 0;
   r = 0;
   r = select (largest + 1, &fds, NULL, NULL, &tv);

   if (r > 0)
   {
      if (p_top)
	 for (scan = p_top; scan; scan = scan->p_next)
	    if (S_FD_ISSET (scan->sock.fd))
	       player_input (scan);
      if (S_FD_ISSET (sock_lis.fd))
	 accept_connection ();
   }
#endif
   if (p_top)
      for (scan = p_top; scan; scan = scan->p_next)
      {
	 if (scan->flags & P_IDLE)	// specified disconnections

	 {
	    vlog ("idle", "Disconnecting %s due to idling.\n", scan->name);
	    destroy_player (scan);

	 }

	 if (scan->buffer.age > 0)
	    buffer_send (scan);

	 if (scan->flags & P_LOGOUT)
	    destroy_player (scan);

      }

   return;
}

int          main (int c, char *v[])
{
   char        *sanity;
   char         program[20];
   int          opt, i;
   pid_t        pid;

#if DUALPLEX
   int          x;

#endif

   strncpy(program, v[0], 19);
   if (c < 2)
   {
      system_flags |= NO_FORK;
   }
   if(!(system_flags & NO_FORK)) {
   pid = fork ();
   switch (pid)
   {
   case -1:
      printf (" -=> Fork failure (-1)!\n");
      exit (0);
      break;
   case 0:
      break;
   default:
      exit (0);
      break;
   }
   }
   
   system_flags |= ECHO_LOGS;
   system_flags |= HEALTHY;
   system_flags |= HIDDEN;
   s_info.pid = getpid ();
   sync_pid (s_info.pid);
   stack = (char *) malloc (STACK_LEN);
   if (!stack)
   {
      abort_boot ("Bad stack malloc.\n");
      return -1;
   }
   memset (stack, 0, STACK_LEN);
   sanity = stack;

   while ((opt = getopt (c, v, "lnrshvx")) != -1)
   {
      switch (opt)
      {
      case 'x':
	 break;
      case 'l':
	 system_flags |= LOCKOUT;
	 printf (" -=> Locking out the system!\n");
	 break;
      case 'n':
	 system_flags |= NO_NEWBIES;
	 printf (" -=> Closing system to newbies.\n");
	 break;
      case 'r':
	 system_flags |= TEMP_CLOSED;
	 printf (" -=> Closing system to residents.\n");
	 break;
      case 's':
	 system_flags &= ~ECHO_LOGS;
	 break;
      case 'h':
	 system_flags &= ~HIDDEN;
	 printf (" -=> Running without HIDDEN flags.\n");
	 break;
      case 'v':
	 system_flags &= ~HEALTHY;
	 printf (" -=> Running in vunerable mode!\n");
	 break;
      case ':':
	 printf ("Option needs value.\n");
	 break;
      case '?':
	 printf ("Unknown option.\n");
	 break;
      }
   }

   if ((uname (&uts)) < 0)
      vlog ("boot", " -=> Couldn't get utsname information!\n");
   else
      vlog ("boot", " -=> System is %s on %s hardware\n -=> Nodename is %s\n -=> Version is %s, %s\n", uts.sysname, uts.machine, uts.nodename, uts.release, uts.version);

   if (system_flags & HIDDEN)
   {
      vlog ("boot", " -=> Setting up system name.\n");
      for (i = c - 1; i >= 0; i--)
	 memset (v[i], 0, strlen (v[i]));

#if DUALPLEX
      sprintf (v[0], "-=> Serpent Dualplex (%s v%s)", RELEASE, VERSION);
#else
      sprintf (v[0], "-=> Serpent (%s v%s)", RELEASE, VERSION);
#endif
   }
   vlog ("boot", " -=> Registering boot date.\n");
   s_info.start_date = time (0);
   strftime (s_info.boot_date, 49, "%I:%M:%S %p on %A %d %B %Y",
	     localtime (&s_info.start_date));
   strcpy (s_info.max_login_name, "No-one!");
   vlog ("boot", " -=> Configuring emulations.\n");
   init_termtypes ();
   init_signals ();
   init_timer ();
   vlog ("boot", " -=> Caching command keys.\n");
   init_parser ();
   init_command_modules ();
   init_public_rooms ();
#if DUALPLEX
   vlog ("boot", " -=> Registering ports.\n");
   s_info.port[0] = (CONFIG_PORT);
   s_info.port[1] = (DUAL_PORT);
   for (x = 0; x < 2; x++)
      sock_listen (&sock_lis[x], s_info.port[x]);
#else
   vlog ("boot", " -=> Registering port.\n");
   sock_listen (&sock_lis, CONFIG_PORT);
#endif
   init_dfiles ();
   vlog ("system", " -=> Server is alive and kicking.\n");
   
   if(system_flags & NO_FORK)
     {
	printf("\n *** You have booted in debugging mode (gdm) ***\n"
	       " *** If you wish to boot normally, please use:\n"
	       " `%s x`\n"
	       " *** If you wish to kill this process, send the\n"
	       " *** Terminate signal with `kill -9 %d`\n\n", program, s_info.pid);
     }
  
   while (!(system_flags & SHUTDOWN))
   {
      errno = 0;
      if (sanity != stack)
      {
	 sprintf (sanity, "main: reclaimed %d bytes of stack.\n",
		  (int) stack - (int) sanity);
	 log ("stack", sanity);
	 stack = sanity;
      }
      core_loop ();
   }
   shutdown_server ();
   return 0;
}
