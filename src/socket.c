//////////////////////////////////////////////////////////////////
// File: socket.c                                               //
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
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <arpa/telnet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdarg.h>
#include <pthread.h>

#include "config.h"
#include "flags.h"
#include "proto.h"
#include "structs.h"
#include "ansi.h"

extern command *root_commands[];
#if DUALPLEX
extern struct sock sock_lis[];

#else
extern struct sock sock_lis;

#endif

int          player_read_char (player * p, char *i)
{
 again:
   if (read (p->sock.fd, i, sizeof (char)) < 0)
   {
      switch (errno)
      {
      case EINTR:
      case EAGAIN:
	 goto again;
	 break;
      case EINVAL:
      case EPIPE:
      case EBADF:
	 p->flags |= P_LOGOUT;
	 vlog ("read", "Logging player %s out.\n", p->name);
	 return (-1);
	 break;
      default:
	 vlog ("read", "nothing handled.\n");
	 break;
      }
   }
   return (sizeof (char));
}

char        *color_parser (player * p, char col)
{
   switch (col)
   {
   case 'r':
      return FG_RED;
      break;
   case 'R':
      return FG_LT_RED;
      break;
   case 'g':
      return FG_GREEN;
      break;
   case 'G':
      return FG_LT_GREEN;
      break;
   case 'y':
      return FG_BROWN;
      break;
   case 'Y':
      return FG_YELLOW;
      break;
   case 'b':
      return FG_BLUE;
      break;
   case 'B':
      return FG_LT_BLUE;
      break;
   case 'p':
      return FG_MAGENTA;
      break;
   case 'P':
      return FG_LT_MAGENTA;
      break;
   case 'c':
      return FG_CYAN;
      break;
   case 'C':
      return FG_LT_CYAN;
      break;
   case 'w':
      return FG_DK_GRAY;
      break;
   case 'W':
      return FG_LT_GRAY;
      break;
   case 'k':
   case 'K':
      return FG_BLACK;
      break;
   case 't':
   case 'T':
      return FG_WHITE;
      break;
   case 'n':
   case 'N':
      return NOR;
      break;
   case 'V':			// Hard-coded varibles for dynamic strings

      return VERSION;
      break;
   case 'E':
      return RELEASE;
      break;
   case 'M':
      return s_info.time;
   case '^':
      return "^";
      break;
   default:
      return NOR;
      break;
   }
   return NOR;
}

void         sock_init (struct sock *s)
{
   s->fd = -1;
   memset (&s->socket, '\0', sizeof (s->socket));
   s->in.packets = 0;
   s->in.bytes = 0;
   s->out.packets = 0;
   s->out.bytes = 0;
   s->host[0] = 0;
}

void         sock_listen (struct sock *s, int port)
{
   int          dummy = 1;

   sock_init (s);

   s->fd = (int) socket (AF_INET, SOCK_STREAM, 6);
   setsockopt (s->fd, SOL_SOCKET, SO_REUSEADDR, &dummy, 0);

   s->socket.sin_family = AF_INET;
   s->socket.sin_port = htons (port);
   s->socket.sin_addr.s_addr = htonl (INADDR_ANY);

   if (bind (s->fd, (struct sockaddr *) &s->socket, sizeof (s->socket)) < 0)
   {
      vlog ("boot", "Bind failed to port %d.  Reason: %s.\n", port,
	    strerror (errno));
      fatal ("Failed to bind main fd.\n");
      return;
   }

   if (listen (s->fd, 20) < 0)
   {
      vlog ("boot", "Failed to listen on port %d. Reason: %s\n", port,
	    strerror (errno));
      fatal ("Failed to listen on main fd.\n");
      return;
   }

   vlog ("boot", " -=> Alive and kicking on port %d.\n", port);
   return;
}

void        *sock_resolve (void *v)
{
   struct sock *s = v;
   struct hostent *tmp;
   static int   inuse = 0;

   while (inuse)
      usleep (50);

   inuse = 1;

   if (SLOW_DNS == NO)
      tmp = gethostbyaddr ((char *) &s->socket.sin_addr,
			   sizeof (s->socket.sin_addr), AF_INET);
   if (tmp && SLOW_DNS == NO)
      strncpy (s->host, tmp->h_name, 255);
   else
      strncpy (s->host, s->ip, 255);

   inuse = 0;
   return (NULL);
}

int          sock_accept (struct sock *l, struct sock *s)
{
   int          nfd;
   int          i;

   i = sizeof (struct sockaddr_in);

   s->fd = accept (l->fd, (struct sockaddr *) &s->socket, &i);
   nfd = dup (s->fd);
   close (s->fd);
   s->fd = nfd;
   i = 1;
   setsockopt (s->fd, SOL_SOCKET, SO_REUSEADDR, &i, 0);

   ioctl (s->fd, FIONBIO, &i);
   if (s->fd < 0)
      s->fd = -1;
   strcpy (s->ip, inet_ntoa (s->socket.sin_addr));
   s->port = ntohs (s->socket.sin_port);
   sock_resolve (s);
   s_info.connections++;
   if (s_info.connections > s_info.max_logins)
      s_info.max_logins = s_info.connections;
   s_info.logins++;
   return (0);
}

int          sock_write (int sockfd, char *buf, size_t count)
{
   size_t       bytes_sent = 0;
   int          this_write;

   while (bytes_sent < count)
   {
      do
	 this_write = write (sockfd, buf, count - bytes_sent);
      while ((this_write < 0) && (errno == EINTR)
	     && (errno != EBADF) && (errno != EPIPE));
      if (this_write <= 0)
	 return this_write;
      bytes_sent += this_write;
      buf += this_write;
   }
   return bytes_sent;
}

int          sock_send (struct sock *s, char *format)
{
   int          r;

   if (s->fd == -1)
      return (0);
   r = sock_write (s->fd, format, strlen (format));

   if (r < 0)
      switch (errno)
      {
      case EINTR:
      case EAGAIN:
	 r = sock_write (s->fd, format, strlen (format));
	 break;
      case EINVAL:
      case EBADF:
      case EPIPE:

	 return (-1);
	 break;
      default:
	 break;
      }

   s->out.packets++;
   s->out.bytes += r;
   s_info.apps++;
   s_info.abps += r;
   s_info.out.packets++;
   s_info.out.bytes += r;
   return (r);
}

#if DUALPLEX
int          accept_connection (int dual)
#else
int          accept_connection (void)
#endif
{
   player      *p;

   p = malloc_player ();
   sock_init (&p->sock);
#if DUALPLEX
   p->dual = dual;
   sock_accept (&sock_lis[dual], &p->sock);
#else
   sock_accept (&sock_lis, &p->sock);
#endif
   if (p->sock.fd == -1)
      return (0);

   // test write before we do anything else
   if ((sock_send (&p->sock, "\n\rInitalising your connection ...\n\r")) < 0)
   {
      shutdown (p->sock.fd, 2);
      close (p->sock.fd);
      p->sock.fd = -1;
      vlog ("error", "Destroying player %s. Bad test write.\n", p->name);
      if (p)
	 free (p);
   }
   p->id = new_pid ();
   link_player (p);
   p->login_time = time (0);
   login_screen (p);
   return (0);
}

int          player_input (player * p)
{
   int          l = 0;
   char         i, r;

   if (ioctl (p->sock.fd, FIONREAD, &l) < 0)
   {
      switch (errno)
      {
      case EBADF:
	 p->flags |= P_LOGOUT;
	 vlog ("read", "not valid\n");
	 break;
      case ENOTTY:
	 p->flags |= P_LOGOUT;
	 vlog ("read", "char specific\n");
	 break;
      case EINVAL:
	 p->flags |= P_LOGOUT;
	 vlog ("read", "not valid\n");
      default:
	 p->flags |= P_LOGOUT;
	 vlog ("read", "eeep!\n");
      }
   }

   p->sock.in.packets++;
   p->sock.in.bytes += l;
   s_info.in.packets++;
   s_info.in.bytes += l;
   if (!l)
   {
      destroy_player (p);
   }

   while (l--)
   {
      if ((r = player_read_char (p, &i)) > 0)
	 switch (i)
	 {
	 case (char) IAC:
	    player_read_char (p, &i);
	    l--;
	    switch (i)
	    {
	    case (char) AYT:
	       tell_player (p, "Yes, i'm here! Lagging are we?\n");
	       break;
	    case (char) IP:
	       vlog ("read", "Logging player %s out. (char)IP.\n", p->name);
	       p->flags |= P_LOGOUT;
	       break;
	    case (char) EC:
	       do_backspace (p);
	       break;
	    case (char) EL:
	       flush_buffer (p);
	       break;

	    case (char) DO:
	       player_read_char (p, &i);
	       l--;
	       switch (i)
	       {
	       case (char) TELOPT_SGA:
		  p->flags &= ~TEL_GA;
		  tell_player (p, "%c%c%c", IAC, WILL, TELOPT_SGA);
		  break;
	       case (char) TELOPT_EOR:
		  p->flags |= TEL_EOR;
		  tell_player (p, "%c%c%c", IAC, WILL, TELOPT_EOR);
		  break;
	       case (char) TELOPT_ECHO:
		  p->flags |= TEL_ECHO;
		  tell_player (p, "%c%c%c", IAC, WILL, TELOPT_EOR);
		  break;
	       default:
		  tell_player (p, "%c%c%c", (char) IAC, (char) WONT, i);
		  break;
	       }
	       break;		// case (char)DO:

	    case (char) DONT:
	       player_read_char (p, &i);
	       l--;
	       switch (i)
	       {
	       case (char) TELOPT_SGA:
		  p->flags |= TEL_SGA;
		  break;
	       case (char) TELOPT_EOR:
		  p->flags &= ~TEL_EOR;
		  break;
	       case (char) TELOPT_ECHO:
		  p->flags &= ~TEL_ECHO;
		  break;
	       default:
		  break;
	       }
	       break;		// case (char)DONT:

	    case (char) WILL:
	       player_read_char (p, &i);
	       l--;
	       tell_player (p, "%c%c%c", (char) IAC, (char) DONT, i);
	       break;

	    case (char) WONT:
	       break;

	    default:
	       vlog ("iac", "not handled\n");
	       break;
	    }
	    break;

	 case '\n':

	    if (p->func)
	    {
	       (p->func) (p, p->ibuffer);
	    }


	    else if (p->ibuffer[0])
	    {
	       run_command (p, p->ibuffer);
	    }
	    if (!strstr (p->ibuffer, "!"))
	       strncpy (p->redo_buffer, p->ibuffer, 255);
	    flush_buffer (p);
	    if (p->flags & P_PROMPT)
	       promptp (p, p->prompt);
	    if (!(p->flags & P_LAST_R))
	    {
	       p->flags |= (P_LAST_N | P_READY);
	       p->column = 0;
	    }


	    break;

	 default:
	    p->flags &= ~(P_LAST_N | P_LAST_R);
	    if (i == 8 || i == 127)
	       do_backspace (p);
	    else if (i > 31 && i < 127)
	    {
	       if (p->ibuffer_ptr > P_IBUFFER_LEN)
		  p->flags |= P_TRUNC;
	       else if (p->flags & TEL_ECHO)
	       {
		  write (p->sock.fd, (char *) &i, sizeof (i));
		  p->sock.out.packets++;
		  p->sock.out.bytes++;
	       }
	       else
		  p->ibuffer[p->ibuffer_ptr++] = i;

	       p->column++;
	    }
	    break;
	 }
      else
      {

	 vlog ("read", "Logging player %s out.\n", p->name);
	 p->flags |= P_LOGOUT;
	 return (-1);
      }
   }
   return (0);
}

void         do_backspace (player * p)
{
   if (p->ibuffer_ptr)
      p->ibuffer_ptr--;
   p->ibuffer[p->ibuffer_ptr] = '\0';
   /* Move client cursor back one */
   /* Write space to client screen */
   /* Move client cursor back one */
   return;
}

void         set_echo (player * p, int type)
{
   char        *ref = stack;

   *stack++ = (char) IAC;
   switch (type)
   {
   case ECHO_OFF:
      *stack++ = (char) WILL;
      break;
   case ECHO_ON:
      *stack++ = (char) WONT;
   }
   *stack++ = (char) TELOPT_ECHO;
   *stack++ = '\0';
   writep (p, ref);
   stack = ref;
   return;
}


void         flush_buffer (player * p)
{
   memset (p->ibuffer, '\0', P_IBUFFER_LEN);
   p->ibuffer_ptr = 0;
   return;
}


dfile        format_output (player * p, char *str)
{
   dfile        f;
   int          i, bold = 0;

   f.size = 0;
   f.data = stack;
   if (!str || !*str)
      return f;
   if (channel)
   {
      for (; *str == '\n'; *stack++ = *str++);
      for (i = 0; channels[i].key; i++)
	 if (channels[i].flag == channel)
	 {
	    if (channels[i].color)
	    {
	       strcpy (stack, channels[i].color);
	       while (*stack)
	       {
		  stack++;
		  f.size++;
	       }

	    }
	    if (channels[i].tag)
	    {
	       *stack++ = channels[i].tag;
	       *stack++ = ' ';
	    }
	    if (p->termtype && channels[i].bold)
	    {
	       strcpy (stack, term_codes[p->termtype].bold);
	       for (bold = 1; *stack; stack++);
	    }
	    break;
	 }
   }

   while (*str)
   {
      switch (*str)
      {
      case '\n':
	 *stack++ = '\r';
	 *stack++ = *str++;
	 f.size += 2;
	 break;
      default:
	 *stack++ = *str++;
	 f.size++;
	 break;
      }
   }
   strcpy (stack, NOR);
   while (*stack)
   {
      stack++;
      f.size++;
   }
   *stack++ = '\0';
   return f;
}

void         writep (player * p, char *str)
{
   dfile        f;
   char        *oldstack = stack;

   if (!p)
      return;
   if (p->flags & P_LOGOUT)
      return;
   if (p->sock.fd == -1)
      return;
   f = format_output (p, str);
   if (stack == oldstack)
      return;
   buffer_add (p, oldstack);
   stack = oldstack;
   return;
}


void         promptp (player * p, char *str)
{
   char        *ref = stack;

   while (*str)
      *stack++ = *str++;

   *stack++ = (char) IAC;
   *stack++ = (char) EOR;
   *stack++ = '\0';
   buffer_add (p, ref);
   stack = ref;
   return;
}

void         init_termtypes (void)
{
   char        *ref = stack;
   int          i;

   strcpy (stack, "Available terminal types:  ");
   stack = strchr (stack, 0);
   for (i = 0; term_codes[i].key; i++)
   {
      sprintf (stack, "%s  ", term_codes[i].key);
      stack = strchr (stack, 0);
   }
   stack -= 2;
   *stack++ = '\n';
   *stack++ = '\0';
   termtypes_msg = (char *) malloc (strlen (ref) + 1);
   strcpy (termtypes_msg, ref);
   stack = ref;
   return;
}
