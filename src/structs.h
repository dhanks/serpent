//////////////////////////////////////////////////////////////////
// File: structs.c                                              //
//////////////////////////////////////////////////////////////////
// Copyright (c) 1998.  Serpent Development                     //
// Regents to Douglas Hanks.                                    //
//////////////////////////////////////////////////////////////////
// You may reach Serpent Development via email at:              //
// Douglas Hanks: ima@realms.org                                //
// Tom Malcom   : tom@ackphft.matsu.alaska.edu                  //
//////////////////////////////////////////////////////////////////

#if !defined(STRUCTS_H)
        #define STRUCTS_H 1


#include <time.h> 	/* time_t for s_player.login_time */

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define SQUISH(x) (x+1)
#define BUF_SIZE   1024

struct  s_player;
typedef void c_func(struct s_player *, char *);

struct packets
{
   unsigned long packets, bytes;
};

struct system_info
{
   int            port[2];
   int            pid;
   int            flags;
   int            logins;
   int            max_logins;
   int            connections;
   int            uptime;
   int            apps;
   int            abps;
   int            on_since;
   float          pps;
   float          bps;
   char           boot_date[50];
   char           max_login_name[40];
   char           time[50];
   time_t         start_date;
   struct packets in;
   struct packets out;
};

struct sock
{
   int                 fd;
   int                 port;
   struct sockaddr_in  socket;
   struct packets      in;
   struct packets      out;
   char                ip[16];
   char                host[256];
};

typedef struct {
        char            *key;
        int              flag;
        int              bold;
        char             tag;
        int              level;
        char            *color;
} tag_list;

typedef struct {
        char            *key;
        char            *bold;
        char            *reset;
} term_info;
   

typedef struct {
	char 		*name;
	char 		*data;
	int 		 size;
} dfile;


typedef struct {
	char 		*key;
	int 		 bit;
} bit;


struct s_command {
        int               dyn;
	char 	 	 *key;
	void            (*comm_func)(struct s_player *, char *);
        char             *func_name;
	int 		  level;
	int 		  flags;
        struct s_command *sublist;
};

struct s_help {
   char       key[50];
   int        level;
   int        read;
   struct s_help *next;
   struct s_help *prev;
};

struct s_list {
   char           name[100];
   int            flags;
   struct s_list *next;
   struct s_list *prev;
};

typedef struct s_command command;


struct s_parser {
	char 		*key;
	command       ***list;
	int 		 level;
	int 		 count;
};

#define R_NAME_LEN 	15
#define R_LOCATION_LEN 	45
#define R_ENTERMSG_LEN 	50

struct s_room {
	int 		 id;
	char 		 name[SQUISH(R_NAME_LEN)];
	char 		 location[SQUISH(R_LOCATION_LEN)];
	char 		 entermsg[SQUISH(R_ENTERMSG_LEN)];
        char             owner[SQUISH(30)];
	dfile 		 desc;
	int 		 flags;	
	int 		 x;
	int 		 y;
        int              z;
        struct s_room   *n;
        struct s_room   *e;
        struct s_room   *s;
        struct s_room   *w;
        struct s_room   *d;
        struct s_room   *u;
	struct s_room 	*next;
	struct s_room 	*prev;
        struct s_player *top_player;
};                                 

struct s_object
{
   char             name[40];
   char             description[80];
   int              flags;
   int              id;
   struct s_object *next;
   struct s_object *prev;
   struct s_room   *location;
   struct s_player *holder;
};

#define WEAPON       (1<<0)
#define ARMOR        (1<<1)
#define FOOD         (1<<2)
#define DRINK        (1<<3)

#define P_NAME_LEN 	15
#define P_IP_LEN 	16
#define P_IBUFFER_LEN 	256
#define P_TITLE_LEN 	50
#define P_URL_LEN 	50
#define P_EMAIL_LEN 	50
#define P_PROMPT_LEN 	10
#define P_ENTERMSG_LEN 	50
#define P_CONMSG_LEN 	50
#define P_DISCONMSG_LEN 50
#define P_RECONMSG_LEN 	50
#define P_DESCRIPTION_LEN 150

struct s_pub_room
{
   int x;
   int y;
   int z;
   int flags;
   struct s_room *location;
   char name[30];
   char entermsg[50];
   char description[50];
};

struct s_player {
        int              pass;
        int              total_login;
        int              dual;
	int 		 id;
        int              idle;
        int              residency;        
        int              commands_out;
        char             lower_name[SQUISH(P_NAME_LEN)];
	char 		 name[SQUISH(P_NAME_LEN)];
	char 		 password[SQUISH(13)];
	int 		 password_fails;
	time_t 		 login_time;
        time_t           last_seen;
        int              on_since;
	char 		 ip[SQUISH(P_IP_LEN)];
        char             dns[50];
	int 		 port;
        char             redo_buffer[SQUISH(P_IBUFFER_LEN)];
	char 		 ibuffer[SQUISH(P_IBUFFER_LEN)];
	int 		 ibuffer_ptr;
	int 		 column;
	int 		 termx;
	c_func 		*func;
	c_func 		*timeout_func;
	int 		 timeout;
	int 		 flags;
	int 		 level;
	int 		 gender;
        int              termtype;
       	struct s_room	*room;
        struct sock      sock;
	char 		 title[SQUISH(P_TITLE_LEN)];
	char 		 description[SQUISH(P_DESCRIPTION_LEN)];
	char 		 url[SQUISH(P_DESCRIPTION_LEN)];
	char 		 email[SQUISH(P_EMAIL_LEN)];
	char 		 prompt[SQUISH(P_PROMPT_LEN)];
	char 		 entermsg[SQUISH(P_ENTERMSG_LEN)];
	char 		 conmsg[SQUISH(P_CONMSG_LEN)];
	char 		 disconmsg[SQUISH(P_DISCONMSG_LEN)];
	char 		 reconmsg[SQUISH(P_RECONMSG_LEN)];
	struct s_player *p_next;
	struct s_player *p_prev;
        struct s_player *room_next;
        struct s_player *room_prev;
	void 		*tmp;
        struct s_list   *list;
        struct {
	   char              *output;
	   unsigned long int  size, len, age, pos, isize, inc_age;
	   
	} buffer;
	     
};
typedef struct s_list       list;
typedef struct s_help       help;
typedef struct s_room 	    room;
typedef struct s_player     player;
typedef struct s_object     object;

#endif
