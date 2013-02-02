//////////////////////////////////////////////////////////////////
// File: flags.h                                                //
//////////////////////////////////////////////////////////////////
// Copyright (c) 1998.  Serpent Development                     //
// Regents to Douglas Hanks.                                    //
//////////////////////////////////////////////////////////////////
// You may reach the Serpent Development email at:              //
// Douglas Hanks: ima@realms.org                                //
// Tom Malcom   : tom@ackphft.matsu.alaska.edu                  //
//////////////////////////////////////////////////////////////////

#if!defined(FLAGS_H)
        #define FLAGS_H 1

#define YES 1
#define NO  0

#define BASE            (1<<0)
#define RESIDENT        (1<<1)
#define PSU             (1<<2)
#define SU              (1<<3)
#define LOWER_ADMIN     (1<<4)
#define ADMIN           (1<<5)
#define HCADMIN         (1<<6)
#define CODER           (1<<7)
#define SYSOP           (1<<7)

/* system_flags */
#define SHUTDOWN 	(1<<0) 	/* Shut down server 		*/
#define ECHO_LOGS 	(1<<1) 	/* Echo all logs to stderr 	*/
#define PATH_ERROR 	(1<<2) 	/* File I/O or Path error 	*/
#define LOCKOUT         (1<<3)  /* Total system lockout         */
#define NO_NEWBIES      (1<<4)  /* Closed to new comers         */
#define TEMP_CLOSED     (1<<5)  /* Temporarily closed           */
#define HEALTHY         (1<<6)  /* Heathly mode                 */
#define HIDDEN          (1<<7)  /* Runs in Hidden mode          */
#define NO_FORK         (1<<8)  /* Tuns without fork()          */
/* command.flags */
#define c_NSP 		(1<<0) 	/* No space between command and params */
#define c_ALS 		(1<<1) 	/* Alias for another command 	*/
#define c_DIS           (1<<2)  /* Disabled                     */

/* set_echo(int type) */
#define ECHO_OFF 	1 	/* Disable client echo 		*/
#define ECHO_ON  	2 	/* Enable  client echo 		*/

/* s_player.flags */
#define P_LOGOUT 	(1<<0) 	/* Log out on next cycle 	*/
#define P_LAST_N 	(1<<1) 	/* Last character read was \n 	*/
#define P_LAST_R 	(1<<2) 	/* Last character read was \r 	*/
#define P_TRUNC 	(1<<3) 	/* Input buffer was truncated 	*/
#define P_PROMPT 	(1<<4) 	/* Send command prompt 		*/
#define P_READY 	(1<<5) 	/* Send to parser on next cycle */
#define P_RECON         (1<<6)  /* Reconnection toggle          */
#define TEL_GA          (1<<7)
#define TEL_EOR         (1<<8)
#define TEL_ECHO        (1<<9)
#define TEL_SGA         (1<<10)
#define HERE            (1<<11)
#define P_IDLE          (1<<12)
#define SHOW_EXITS      (1<<13)
#define SHOW_MAPS       (1<<14)

/* s_room.flags */
#define R_BLOCK_N 	(1<<0) 	/* Block exit north 		*/
#define R_BLOCK_S 	(1<<1) 	/* Block exit south 		*/
#define R_BLOCK_E 	(1<<2) 	/* Block exit east 		*/
#define R_BLOCK_W 	(1<<3) 	/* Block exit west 		*/
#define R_BLOCK_U       (1<<4)  /* Block exit up                */
#define R_BLOCK_D       (1<<5)  /* Block exit down              */
#define R_MAIN          (1<<6)  /* Main room                    */
#define R_VOID          (1<<7)  /* Void room                    */
#define R_NOFLAGS       (1<<8)  /* No flags                     */

/* s_player.gender */
#define MALE 		1
#define FEMALE 		2
#define NEUTER 		0

/* channel values */
#define CHAN_TELLS 	1
#define CHAN_ROOM       2
#define CHAN_SHOUTS     3
#define CHAN_ECHO       4
#define CHAN_SYSTEM     5
#define CHAN_SU         6
#define CHAN_ADMIN      7

/* list flags */
#define L_FRIEND       (1<<0)
#define L_INFORM       (1<<1)
#define L_BEEP         (1<<2)
#endif
