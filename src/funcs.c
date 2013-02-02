//////////////////////////////////////////////////////////////////
// File: funcs.c                                                //
//////////////////////////////////////////////////////////////////
// Copyright (c) 1998.  Serpent Development                     //
// Regents to Douglas Hanks.                                    //
//////////////////////////////////////////////////////////////////
// You may reach the Serpent Development via email at:          //
// Douglas Hanks: ima@realms.org                                //
// Tom Malcom   : tom@ackphft.matsu.alaska.edu                  //
//////////////////////////////////////////////////////////////////

#include <ctype.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "config.h"
#include "flags.h"
#include "proto.h"
#include "structs.h"

char        *w_units[] =
{"zero", "one", "two", "three", "four", "five", "six",
 "seven", "eight", "nine"};
char        *w_teens[] =
{"ten", "eleven", "twelve", "thirteen", "fourteen",
 "fifteen", "sixteen", "seventeen", "eighteen",
 "nineteen"};
char        *w_tens[] =
{"ten", "twenty", "thirty", "fourty", "fifty", "sixty",
 "seventy", "eighty", "ninety"};


char        *end_string (char *str)
{
   str = strchr (str, 0);
   return ++str;
}

void         lower_case (char *str)
{
   while (*str)
      *str++ = tolower (*str);
}

char        *next_space (char *str)
{
   for (; *str && *str != ' ' && *str != '\t'; str++);
   if (*str == ' ' || *str == '\t')
   {
      for (; *str && (*str == ' ' || *str == '\t'); str++);
      str--;
   }
   return str;
}


char        *trim_end (char *str)
{
   char        *trail = strchr (str, 0);

   if (*(--trail))
      for (; *trail == ' ' || *trail == '\t'; *trail-- = '\0');
   return str;
}


char         asks (char *str)
{
   while (*str)
      str++;
   if (*(str - 1))
      return *(--str);
   return 0;
}

char        *word_time (int t)
{
   static char  time_string[100], *fill;
   int          days, hrs, mins, secs;

   if (!t)
      return "no time at all";
   days = t / 86400;
   hrs = (t / 3600) % 24;
   mins = (t / 60) % 60;
   secs = t % 60;
   fill = time_string;
   if (days)
   {
      sprintf (fill, "%d day", days);
      while (*fill)
	 fill++;
      if (days != 1)
	 *fill++ = 's';
      if (hrs || mins || secs)
      {
	 *fill++ = ',';
	 *fill++ = ' ';
      }
   }
   if (hrs)
   {
      sprintf (fill, "%d hour", hrs);
      while (*fill)
	 fill++;
      if (hrs != 1)
	 *fill++ = 's';
      if (mins && secs)
      {
	 *fill++ = ',';
	 *fill++ = ' ';
      }
      if ((mins && !secs) || (!mins && secs))
      {
	 strcpy (fill, " and ");
	 while (*fill)
	    fill++;
      }
   }
   if (mins)
   {
      sprintf (fill, "%d minute", mins);
      while (*fill)
	 fill++;
      if (mins != 1)
	 *fill++ = 's';
      if (secs)
      {
	 strcpy (fill, " and ");
	 while (*fill)
	    fill++;
      }
   }
   if (secs)
   {
      sprintf (fill, "%d second", secs);
      while (*fill)
	 fill++;
      if (secs != 1)
	 *fill++ = 's';
   }
   *fill++ = 0;
   return time_string;
}

char        *logtime (void)
{
   static char  ret[18];
   time_t       t = time (0);

   strftime (ret, 17, "%b %d %H:%M:%S", localtime (&t));
   return ret;
}

char        *string_time (time_t t)
{
   static char  ret[18];

   strftime (ret, 17, "%b %d %H:%M:%S", localtime (&t));
   return ret;
}

void         log (char *filename, char *str)
{
   FILE        *f;

   sprintf (stack, "%slogs/%s", SERVER_ROOT, filename);
   f = fopen (stack, "a");
   if (!*str)
      str = "Null string sent to logging call!\n";
   if (!f)
      printf ("Failed to open logs/%s for writing\n", filename);
   else
   {
      fprintf (f, "%s - %s", logtime (), str);
   }
   fclose (f);

   if (system_flags & ECHO_LOGS)
      fprintf (stderr, str);
   return;
}


dfile        load_dfile (char *path)
{
   int          fd, err = 0, len;
   dfile        f;

   if ((fd = open (path, O_RDONLY)) < 0)
   {
      vlog ("files", "load_dfile: Failed to open %s\n", path);
      err = 1;
   }
   else
   {
      f.size = lseek (fd, 0, SEEK_END);
      lseek (fd, 0, SEEK_SET);
      f.data = (char *) malloc (f.size + 1);
      if (!f.data)
      {
	 fatal ("load_dfile: Bad malloc.\n");
	 return f;
      }
      memset (f.data, 0, f.size + 1);
      len = read (fd, f.data, f.size);
      if (len < f.size)
      {
	 vlog ("files", "load_dfile: Error after %d bytes of %s\n", len, path);
	 free (f.data);
	 err = 1;
      }
   }
   if (err)
   {
      f.data = (char *) malloc (1);
      if (!f.data)
      {
	 fatal ("load_dfile: Bad malloc.\n");
	 return f;
      }
      *(char *) f.data = '\0';
      f.size = 0;
   }

   f.name = strdup (path);
   close (fd);
   return f;
}


void         vwritep (player * p, char *format,...)
{
   va_list      args;
   char        *ref = stack;

   va_start (args, format);
   vsprintf (stack, format, args);
   va_end (args);
   stack = end_string (ref);
   writep (p, ref);
   stack = ref;
   return;
}


void         raw_wall (char *format,...)
{
   va_list      args;
   char        *ref = stack;

   va_start (args, format);
   vsprintf (stack, format, args);
   va_end (args);
   stack = end_string (ref);
   writea (ref);
   stack = ref;
   return;
}

void         tell_player (player * p, char *format,...)
{
   va_list      args;
   char        *ref = stack;

   va_start (args, format);
   vsprintf (stack, format, args);
   va_end (args);
   stack = end_string (ref);
   writep (p, ref);
   stack = ref;
   return;
}


void         ad_wall (char *format,...)
{
   va_list      args;
   char        *ref = stack;
   player      *scan;

   *stack++ = '[';
   *stack++ = 'A';
   *stack++ = 'D';
   *stack++ = ']';
   *stack++ = ' ';
   va_start (args, format);
   vsprintf (stack, format, args);
   va_end (args);
   stack = end_string (ref);
   for (scan = p_top; scan; scan = scan->p_next)
   {
      if (scan->flags & HERE && scan->residency & ADMIN)
	 writep (scan, ref);
   }
   stack = ref;
   return;
}

void         su_wall (char *format,...)
{
   va_list      args;
   char        *ref = stack;
   player      *scan;

   *stack++ = '[';
   *stack++ = 'S';
   *stack++ = 'U';
   *stack++ = ']';
   *stack++ = ' ';
   va_start (args, format);
   vsprintf (stack, format, args);
   va_end (args);
   stack = end_string (ref);
   channel = CHAN_SU;
   for (scan = p_top; scan; scan = scan->p_next)
   {
      if (scan->flags & HERE && scan->residency & PSU)
	 writep (scan, ref);
   }
   channel = 0;
   stack = ref;
   return;
}

void         vlog (char *filename, char *format,...)
{
   va_list      args;
   char        *ref = stack;

   va_start (args, format);
   vsprintf (stack, format, args);
   va_end (args);
   stack = end_string (ref);
   log (filename, ref);
   stack = ref;
   return;
}


char        *bad_name (char *name)
{
   int          i;
   char        *c;

   for (i = 0; reserved_names[i]; i++)
      if (!strcasecmp (name, reserved_names[i]))
	 return "That is a reserved name. Please choose another.\n";
   if (strlen (name) > P_NAME_LEN)
      return "That's too long. Try something shorter.\n";
   if (strlen (name) < 2)
      return "Names must be at least two letters long.\n";
   for (c = name; *c; c++)
      if (!isalpha (*c))
	 return "Names may only contain alphabetic characters (ie. letters)\n";
   return 0;
}

int          get_bit (char *key, bit * list)
{
   for (; list->key; list++)
      if (!strcasecmp (list->key, key))
	 return list->bit;
   return 0;
}


char        *get_key (int num, bit * list)
{
   for (; list->key; list++)
      if (list->bit == num)
	 return list->key;
   return 0;
}

char        *next_part (char *src, int *size, char c)
{
   int          dummy = *size;

   while (*src && *src != c)
   {
      if (*src == '\r' || *src == '\t')
	 src++;
      if (*src == '\r' || *src == '\t')
	 src++;
      if (*src == '\r' || *src == '\t')
	 src++;
      dummy--;
      *stack++ = *src++;
   }
   src++;
   *size = --dummy;
   *stack++ = '\0';
   return src;
}

char        *get_line (char *src, int *size)
{
   return next_part (src, size, '\n');
}

void         writer (player * p, player * p2, room * r, char *str)
{
   player      *scan;

   if (!r)
      return;
   if (!(p || p2))
   {
      for (scan = p_top; scan; scan = scan->p_next)
	 if (scan->room == r)
	    writep (scan, str);
   }
   else
   {
      for (scan = p_top; scan; scan = scan->p_next)
	 if (!(scan == p || scan == p2) && scan->room == r)
	    writep (scan, str);
   }

   return;
}


void         vwriter (player * p, player * p2, room * r, char *format,...)
{
   va_list      args;
   char        *ref = stack;

   va_start (args, format);
   vsprintf (stack, format, args);
   va_end (args);
   stack = end_string (ref);
   writer (p, p2, r, ref);
   stack = ref;
   return;
}


char        *num2word (int num)
{
   int          hundreds, tens, teens, units;
   static char  ret[40];
   char        *tmp = ret;

   if (!num)
      return w_units[0];
   if (num > 1000)
   {
      sprintf (ret, "%d", num);
      return ret;
   }
   hundreds = num / 100;
   tens = num / 10;
   teens = (num % 100) - 10;
   units = num % 10;
   if (hundreds)
   {
      strcpy (tmp, w_units[hundreds]);
      while (*tmp)
	 tmp++;
      if (tens || units || teens)
      {
	 strcpy (tmp, " and ");
	 while (*tmp)
	    tmp++;
      }
   }
   if (tens == 1)
   {
      strcpy (tmp, w_teens[teens]);
      while (*tmp)
	 tmp++;
      *tmp++ = '\0';
      return ret;
   }
   if (tens)
   {
      strcpy (tmp, w_tens[tens]);
      while (*tmp)
	 tmp++;
   }
   if (units)
   {
      strcpy (tmp, w_units[units]);
      while (*tmp)
	 tmp++;
   }
   *tmp++ = '\0';
   return ret;
}


void         banner (player * p, char *dest, char *str, char c)
{
   int          i = -1, half;

   if (str)
   {
      if (strlen (str) > (p->termx - 6))
      {
	 *dest++ = c;
	 *dest++ = c;
	 *dest++ = ' ';
	 for (i = 0; i < p->termx - 6; i++, *dest++ = *str++);
	 *dest++ = ' ';
	 *dest++ = c;
	 *dest++ = c;
	 *dest++ = '\n';
	 *dest++ = '\0';
	 return;
      }
      half = (p->termx - (strlen (str) + 3)) >> 1;
      for (i = 0; i < half; i++, *dest++ = c);
      *dest++ = ' ';
      for (i++; *str; i++, *dest++ = *str++);
      *dest++ = ' ';
   }
   for (i++; i < p->termx - 1; i++, *dest++ = c);
   *dest++ = '\n';
   *dest++ = '\0';
   return;
}


char        *hisher (player * p)
{
   switch (p->gender)
   {
   case MALE:
      return "his";
   case FEMALE:
      return "her";
   }
   return "its";
}


char        *himher (player * p)
{
   switch (p->gender)
   {
   case MALE:
      return "him";
   case FEMALE:
      return "her";
   }
   return "it";
}


char        *heshe (player * p)
{
   switch (p->gender)
   {
   case MALE:
      return "he";
   case FEMALE:
      return "she";
   }
   return "it";
}


char        *capfirst (char *str)
{
   static char  ret[1024];

   ret[1023] = '\0';
   strncpy (ret, str, 1022);
   ret[0] = toupper (ret[0]);
   return ret;
}


void         get_int (char **psrc, int *dest)
{
   int          i;
   char        *src;
   union
   {
      char         c[4];
      int          i;
   }
   u;

   src = *psrc;
   for (i = 0; i < 4; i++)
      u.c[i] = *src++;
   *dest = ntohl (u.i);
   *psrc = src;
   return;
}


void         put_int (char **pdest, int src)
{
   int          i;
   char        *dest;
   union
   {
      char         c[4];
      int          i;
   }
   u;

   dest = *pdest;
   u.i = htonl (src);
   for (i = 0; i < 4; i++)
      *dest++ = u.c[i];
   *pdest = dest;
   return;
}


void         get_string (char **psrc, char *dest)
{
   char        *src;

   src = *psrc;
   while (*src)
      *dest++ = *src++;
   *dest++ = '\0';
   *psrc = ++src;
   return;
}


void         put_string (char **pdest, char *src)
{
   char        *dest;

   dest = *pdest;
   while (*src)
      *dest++ = *src++;
   *dest++ = '\0';
   *pdest = dest;
   return;
}


void         writea (char *str)
{
   player      *scan;

   for (scan = p_top; scan; scan = scan->p_next)
      writep (scan, str);
   return;
}


void         dump_dfile (dfile f)
{
   int          fd, size;
   char         location[80];

   sprintf(location, "%stmp/dfile", SERVER_ROOT);
   if (!*(f.name))
   {
      log ("files", "dump_dfile: dfile has no name!\n");
      return;
   }
   fd = open (location, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
   if (fd < 0)
      log ("files", "Unable to open tmp/dfile for writing.\n");
   else
   {
      size = write (fd, f.data, f.size);
      close (fd);
      if (size < f.size)
	 vlog ("files", "dump_dfile: Error after %d bytes writing tmp/dfile.\n", size);
      else
      {
	 rename (location, f.name);
	 vlog ("files", "dump_dfile: Wrote %s (%d bytes)\n", f.name, size);
      }
   }
   return;
}
