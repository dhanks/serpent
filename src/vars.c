//////////////////////////////////////////////////////////////////
// File: vars.c                                                 //
//////////////////////////////////////////////////////////////////
// Copyright (c) 1998.  Serpent Development                     //
// Regents to Douglas Hanks.                                    //
//////////////////////////////////////////////////////////////////
// You may reach Serpent Development via email at:              //
// Douglas Hanks: ima@realms.org                                //
// Tom Malcom   : tom@ackphft.matsu.alaska.edu                  //
//////////////////////////////////////////////////////////////////

#include <sys/utsname.h>

#include "ansi.h"
#include "config.h"
#include "flags.h"
#include "structs.h"

struct system_info s_info;
struct utsname uts;
char        *stack = 0;

void        *command_handle;
int          shutdown_count = -1;
int          SLOW_DNS = NO;
int          network_count = 60;
int          system_flags;
int          channel = 0;
player      *p_top;
char        *reserved_names[] =
{"config", "room", 0};
char        *termtypes_msg = 0;
player      *current_player;
char        *current_func;

room        *r_top, *mainroom, *voidroom;

int          timer = 0;

dfile        connect_msg, motd_msg, dis_msg, terminals_msg, screen_one_msg,
             compiled_msg;

tag_list     channels[] =
{
   {"tells", CHAN_TELLS, 1, '>', 0, 0},
   {"room", CHAN_ROOM, 0, '-', 0, 0},
   {"shouts", CHAN_SHOUTS, 0, '!', 0, 0},
   {"echos", CHAN_ECHO, 0, '+', 0, 0},
   {"system", CHAN_SYSTEM, 0, '%', 0, FG_LT_RED},
   {"su", CHAN_SU, 0, ']', 0, FG_YELLOW},
   {"admin", CHAN_ADMIN, 0, '}', 0, FG_LT_CYAN},
   {0, 0, 0, 0}
};

tag_list     root_users[] =
{
   {"admin", 0, 0, 0, 0, 0},
   {0, 0, 0, 0, 0}
};

tag_list     defined_levels[] =
{
   {"newbie", BASE, 0, 0, 0, 0},
   {"resident", RESIDENT, 0, 0, 0, 0},
   {"psu", PSU, 0, 0, 0, 0},
   {"su", SU, 0, 0, 0, 0},
   {"la", LOWER_ADMIN, 0, 0, 0, 0},
   {"admin", ADMIN, 0, 0, 0, 0},
   {"hcadmin", HCADMIN, 0, 0, 0, 0},
   {"coder", CODER, 0, 0, 0, 0},
   {"sysop", SYSOP, 0, 0, 0, 0},
   {0, 0, 0, 0, 0},
};

term_info    term_codes[] =
{
   {"none", 0, 0},
   {"vt100", "\033[1m", "\033[m"},
   {"ansi", "\033[1m", "\033[0m"},
   {0, 0, 0}
};
