



#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <alloca.h>
#include <malloc.h>
#include <zlib.h>
#include <ctype.h>
#include <errno.h>

#include "structs.h"
#include "proto.h"
#include "flags.h"

void        *xfer_int (int save, void *buf, int *data)
{
   int         *l = (int *) buf;

   if (save)
      *l = htons (*data);
   else
      *data = ntohs (*l);
   return ((char *) l + sizeof (int));
}

void        *xfer_long (int save, void *buf, long *data)
{
   long        *l = (long *) buf;

   if (save)
      *l = htons (*data);
   else
      *data = ntohs (*l);
   return ((char *) l + sizeof (long));
}

void        *xfer_string (int save, void *buf, char *data)
{
   char        *l = (char *) buf;

   if (save)
      strcpy (l, data);
   else
      strcpy (data, l);
   return ((char *) l + strlen (data) + 1);
}

int          version = 5;

void         save_player (player * p, char *str)
{
   char        *fn = alloca (256);
   int          fd, s = 1;
   gzFile       gf;
   char        *w, *wbuf;

   if (!(p->email[0] || p->password[0]))
   {
      writep (p, " You must first set a proper email address and password before you may save your character.\n");
      return;
   }

   sprintf (fn, "%slib/players/%s", SERVER_ROOT, p->lower_name);
   fd = open (fn, O_CREAT | O_RDWR | O_TRUNC, 0666);
   gf = gzdopen (fd, "wb9");
   wbuf = w = alloca (sizeof (player));

   w = xfer_int (s, w, &version);
   w = xfer_int (s, w, &p->total_login);
   w = xfer_int (s, w, &p->residency);
   w = xfer_long (s, w, &p->last_seen);
   w = xfer_int (s, w, &p->flags);
   w = xfer_int (s, w, &p->level);
   w = xfer_int (s, w, &p->gender);
   w = xfer_int (s, w, &p->termtype);
   w = xfer_int (s, w, &p->password_fails);

   w = xfer_string (s, w, p->ip);
   w = xfer_string (s, w, p->dns);
   w = xfer_string (s, w, p->lower_name);
   w = xfer_string (s, w, p->name);
   w = xfer_string (s, w, p->password);
   w = xfer_string (s, w, p->title);
   w = xfer_string (s, w, p->description);
   w = xfer_string (s, w, p->url);
   w = xfer_string (s, w, p->email);
   w = xfer_string (s, w, p->prompt);
   w = xfer_string (s, w, p->entermsg);
   w = xfer_string (s, w, p->conmsg);
   w = xfer_string (s, w, p->disconmsg);
   w = xfer_string (s, w, p->reconmsg);

   gzwrite (gf, wbuf, (w - wbuf));
   gzclose (gf);

   channel = CHAN_SYSTEM;
   writep (p, " Data saved to disk.\n\n");
   channel = 0;
   return;
}

int          player_load (player * p)
{
   char        *fn = alloca (256);
   int          fd, s = 0;
   int          version;
   gzFile       gf;
   char        *w, *wbuf;

   sprintf (fn, "%slib/players/%s", SERVER_ROOT, p->lower_name);
   fd = open (fn, O_RDWR);

   if (fd < 0)
      return (0);

   gf = gzdopen (fd, "rb9");
   wbuf = w = alloca (sizeof (player));
   gzread (gf, wbuf, sizeof (player));
   gzclose (gf);

   w = xfer_int (s, w, &version);

   if (version == 2)
   {
      w = xfer_int (s, w, &p->total_login);
      w = xfer_int (s, w, &p->residency);
      w = xfer_long (s, w, &p->last_seen);
      w = xfer_int (s, w, &p->flags);
      w = xfer_int (s, w, &p->level);
      w = xfer_int (s, w, &p->gender);
      w = xfer_int (s, w, &p->termtype);
      w = xfer_int (s, w, &p->password_fails);

      w = xfer_string (s, w, p->ip);
      w = xfer_string (s, w, p->dns);
      w = xfer_string (s, w, p->lower_name);
      w = xfer_string (s, w, p->name);
      w = xfer_string (s, w, p->password);
      w = xfer_string (s, w, p->title);
      w = xfer_string (s, w, p->description);
      w = xfer_string (s, w, p->url);
      w = xfer_string (s, w, p->email);
      w = xfer_string (s, w, p->prompt);
      w = xfer_string (s, w, p->entermsg);
      w = xfer_string (s, w, p->conmsg);
      w = xfer_string (s, w, p->disconmsg);
      w = xfer_string (s, w, p->reconmsg);
      w = xfer_string (s, w, fn);
   }
   if (version == 4)
   {
      w = xfer_int (s, w, &p->total_login);
      w = xfer_int (s, w, &p->residency);
      w = xfer_long (s, w, &p->last_seen);
      w = xfer_int (s, w, &p->flags);
      w = xfer_int (s, w, &p->level);
      w = xfer_int (s, w, &p->gender);
      w = xfer_int (s, w, &p->termtype);
      w = xfer_int (s, w, &p->password_fails);

      w = xfer_string (s, w, p->ip);
      w = xfer_string (s, w, p->dns);
      w = xfer_string (s, w, p->lower_name);
      w = xfer_string (s, w, p->name);
      w = xfer_string (s, w, p->password);
      w = xfer_string (s, w, p->title);
      w = xfer_string (s, w, p->description);
      w = xfer_string (s, w, p->url);
      w = xfer_string (s, w, p->email);
      w = xfer_string (s, w, p->prompt);
      w = xfer_string (s, w, p->entermsg);
      w = xfer_string (s, w, p->conmsg);
      w = xfer_string (s, w, p->disconmsg);
      w = xfer_string (s, w, p->reconmsg);
      w = xfer_string (s, w, fn);
   }
   if (version == 5)
   {
      w = xfer_int (s, w, &p->total_login);
      w = xfer_int (s, w, &p->residency);
      w = xfer_long (s, w, &p->last_seen);
      w = xfer_int (s, w, &p->flags);
      w = xfer_int (s, w, &p->level);
      w = xfer_int (s, w, &p->gender);
      w = xfer_int (s, w, &p->termtype);
      w = xfer_int (s, w, &p->password_fails);

      w = xfer_string (s, w, p->ip);
      w = xfer_string (s, w, p->dns);
      w = xfer_string (s, w, p->lower_name);
      w = xfer_string (s, w, p->name);
      w = xfer_string (s, w, p->password);
      w = xfer_string (s, w, p->title);
      w = xfer_string (s, w, p->description);
      w = xfer_string (s, w, p->url);
      w = xfer_string (s, w, p->email);
      w = xfer_string (s, w, p->prompt);
      w = xfer_string (s, w, p->entermsg);
      w = xfer_string (s, w, p->conmsg);
      w = xfer_string (s, w, p->disconmsg);
      w = xfer_string (s, w, p->reconmsg);
      w = xfer_string (s, w, fn);
   }

   p->flags &= ~P_PROMPT;
   p->flags &= ~P_LOGOUT;
   p->flags &= ~P_IDLE;
   version = 5;
   return (1);
}
