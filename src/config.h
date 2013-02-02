//////////////////////////////////////////////////////////////////
// File: config.h                                               //
//       Contains the system's configuration defines.           //
//////////////////////////////////////////////////////////////////
// Copyright (c) 1998.  Serpent Development                     //
// Regents to Douglas Hanks.                                    //
//////////////////////////////////////////////////////////////////
// You may reach Serpent Development via email at:              //
// Douglas Hanks: ima@realms.org                                //
// Tom Malcom   : tom@ackphft.matsu.alaska.edu                  //
//////////////////////////////////////////////////////////////////

#if !defined(CONFIG_H)
        #define CONFIG_H 1
#include "setup.h"
#define DUALPLEX 1
#define TIME   "No time!"
#if DUALPLEX
 #define DUAL_ONE_FD_SET(a) FD_SET(a,&fds_dual_one); if(a>largest[0]) largest[0]=a;
 #define DUAL_ONE_FD_ISSET(a) FD_ISSET(a,&fds_dual_one)
 #define DUAL_ONE_FD_ZERO(a) FD_ZERO(&fds_dual_one); largest[0]=-1
 #define DUAL_TWO_FD_SET(a) FD_SET(a,&fds_dual_two); if(a>largest[1]) largest[1]=a;
 #define DUAL_TWO_FD_ISSET(a) FD_ISSET(a,&fds_dual_two)
 #define DUAL_TWO_FD_ZERO(a) FD_ZERO(&fds_dual_two); largest[1]=-1
#else
 #define S_FD_SET(a) FD_SET(a,&fds); if(a>largest) largest=a;
 #define S_FD_ISSET(a) FD_ISSET(a,&fds)
 #define S_FD_ZERO(a) FD_ZERO(&fds); largest=-1
#endif
#define PING_INTERVAL 10
#define RUNTIME_PATH    SERVER_ROOT
#define STACK_LEN 	STACK_SIZE
#undef 	ALLOW_RESERVED_PORT
#undef 	NO_SPACE_PROMPT
#define FORK_AND_TERMINATE 1
#define STACK_DEBUG 	1
#define MAPX 		5
#define MAPY 		5
#define P_MAX_ID 	10000
#define R_MAX_ID 	10000
#define TICK_RATE 	15
#define LINE_LEN 	80
#define BANNER_CHAR 	'-'
#define CACHE_SIZE 	20
#define DEF_TERMX 	78
#define MAX_ATTEMPTS 	3
#define CLICK_RATE 	5
#define CONFIG_PATH 	RUNTIME_PATH
#define CONFIG_PORT 	SERVER_PORT
#if DUALPLEX
 #define DUAL_PORT      7979
#endif
#define CONFIG_EMAIL 	SERVER_EMAIL
#define RELEASE         "Cobra"

#endif
