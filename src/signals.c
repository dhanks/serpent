
//////////////////////////////////////////////////////////////////
// File: signals.c                                              //
//////////////////////////////////////////////////////////////////
// Copyright (c) 1998.  Serpent Development                     //
// Regents to Douglas Hanks.                                    //
//////////////////////////////////////////////////////////////////
// You may reach Serpent Development via email at:              //
// Douglas Hanks: ima@realms.org                                //
// Tom Malcom   : tom@ackphft.matsu.alaska.edu                  //
//////////////////////////////////////////////////////////////////

#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include "config.h"
#include "flags.h"
#include "proto.h"
#include "structs.h"

void         do_shutdown (void)
{
   switch (shutdown_count)
   {
   case (3600):
   case (1800):
   case (900):
   case (300):
   case (60):
   case (30):
   case (10):
   case (5):
   case (1):
      channel = CHAN_SYSTEM;
      raw_wall (" -=> System set to shutdown in %s.\n",
		word_time (shutdown_count));
      channel = 0;
      break;
   }
   shutdown_count--;
   if (shutdown_count == 0)
   {
      shutdown_count = -1;
      system_flags |= SHUTDOWN;
   }
   return;
}

void         timer_click (void)
{
   player      *scan;
   static int   network_count = 5;

   if (p_top)
      for (scan = p_top; scan; scan = scan->p_next)
      {
	 scan->idle++;
	 scan->on_since++;
	 if (scan->idle == 1600)
	 {
	    tell_player (scan, " -=> Warning.  You are 30 minutes idle.\007\n");

	 }
	 if (scan->idle > 3600)
	 {
	    tell_player (scan, " -=> Warning.  You are an hour idle.  Bye.\007\n");
	    scan->idle = 0;
	    scan->flags |= P_IDLE;
	 }
	 if (scan->on_since > s_info.on_since)
	 {
	    s_info.on_since = scan->on_since;
	    strcpy (s_info.max_login_name, scan->name);
	 }

      }				// player scan

   if (shutdown_count >= 0)
      do_shutdown ();
   if (network_count > 0)
      network_count--;
   if (network_count == 0)
   {
      network_count = 5;
      s_info.pps = (float) (s_info.apps / 5);
      s_info.bps = (float) (s_info.abps / 5);
      s_info.apps = 0;
      s_info.abps = 0;
   }

   s_info.uptime++;
   return;
}


void         signal_restore (int sig)
{
   struct sigaction sa;

   sa.sa_handler = SIG_DFL;
   sigaction (sig, &sa, 0);
   return;
}

void         signal_SEGV (int dummy)
{
   if (!(system_flags & HEALTHY))
   {
      signal_restore (SIGSEGV);
      log ("signals", "Segmentation violation - emergency shutdown.\n");
   }
   return;
}
void         signal_EXIT (int dummy)
{
   if (!(system_flags & HEALTHY))
   {
      vlog ("signal", "signal caught. exiting cleanly.\n");
      system_flags |= SHUTDOWN;
   }
   return;
}

void         signal_ALRM (int dummy)
{

   if (timer)
      timer--;
   else
   {
      timer_click ();

      timer = CLICK_RATE;
   }
   return;
}

void         init_timer (void)
{
   struct itimerval t;

   vlog ("boot", " -=> Setting timer.\n");
   t.it_interval.tv_usec = 1000000 / CLICK_RATE;
   t.it_interval.tv_sec = 0;
   t.it_value.tv_usec = 1000000 / CLICK_RATE;
   t.it_value.tv_sec = 0;
   if (setitimer (ITIMER_REAL, &t, 0) < 0)
   {
      fatal ("init_timer: Failed to set timer.\n");
      return;
   }

   timer = CLICK_RATE;

   return;
}


void         stop_timer (void)
{
   struct itimerval t;

   t.it_interval.tv_usec = 0;
   t.it_interval.tv_sec = 0;
   t.it_value.tv_usec = 0;
   t.it_value.tv_sec = 0;
   if (setitimer (ITIMER_REAL, &t, 0) < 0)
   {

      fatal ("init_timer: Failed to stop timer.\n");
   }
   else
      log ("system", "stop_timer: Stopped timer.\n");

   return;
}

void         init_signals (void)
{
   struct sigaction sa;

   vlog ("boot", " -=> Starting signal handler.\n");
   sa.sa_flags = 0;
   sa.sa_handler = signal_EXIT;
   sigaction (SIGHUP, &sa, 0);
   sa.sa_handler = signal_EXIT;
   sigaction (SIGINT, &sa, 0);
   sa.sa_handler = signal_EXIT;
   sigaction (SIGQUIT, &sa, 0);
   sa.sa_handler = signal_EXIT;
   sigaction (SIGILL, &sa, 0);
   sa.sa_handler = signal_EXIT;
   sigaction (SIGABRT, &sa, 0);
   sa.sa_handler = signal_EXIT;
   sigaction (SIGFPE, &sa, 0);
   sa.sa_handler = signal_EXIT;
   sigaction (SIGUSR1, &sa, 0);
   sa.sa_handler = signal_SEGV;
   sigaction (SIGSEGV, &sa, 0);
   sa.sa_handler = signal_EXIT;
   sigaction (SIGUSR2, &sa, 0);
   sa.sa_handler = signal_ALRM;
   sigaction (SIGALRM, &sa, 0);
   sa.sa_handler = signal_EXIT;
   sigaction (SIGTERM, &sa, 0);
   sa.sa_handler = signal_EXIT;
   sigaction (SIGCHLD, &sa, 0);
   sa.sa_handler = signal_EXIT;
   sigaction (SIGTRAP, &sa, 0);
   signal (SIGPIPE, SIG_IGN);
   return;
}
