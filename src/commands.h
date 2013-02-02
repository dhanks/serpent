//////////////////////////////////////////////////////////////////
// File: commands.h                                             //
//       Contains prototypes and structs for commands.          //
//////////////////////////////////////////////////////////////////
// Copyright (c) 1998.  Serpent Development.                    //
// Regents to Douglas Hanks.                                    //
//////////////////////////////////////////////////////////////////
// You may reach Serpent Development via email at:              //
// Douglas Hanks: ima@realms.org                                //
// Tom Malcom   : tom@ackphft.matsu.alaska.edu                  //
//////////////////////////////////////////////////////////////////

#if!defined(COMMANDS_H)
	#define COMMANDS_H 1
	#if !defined(STRUCTS_H)
		#include "structs.h"
	#endif
	#if !defined(FLAGS_H)
		#include "flags.h"
	#endif

c_func command_rooms, c_commands, command_look, c_quit, c_null, view_motd, 
       show_exits, show_maps, time_of_day, network_traffic, c_terminal, 
       command_help, command_jump, c_title, c_description, c_url, c_email, 
       c_prompt, display_version, save_player, c_password, c_recap, 
       c_reconmsg, c_entermsg, c_conmsg, c_disconmsg, reload;

c_func go_north, go_east, go_south, go_west, go_down, go_up;

// Format for commands:
// {YES/NO, "key name", function, function name, priv, level, subcommand},

command room_commands[] = {
	{NO,"room", 	0, 0, 0, 0, 0}, 	/* Name of list */
	{NO, 0, 0, 0, 0, 0, 0}
};

command show_commands[] = {
        {NO, "show",        0, 0, 0, 0, 0},
        {NO, "exits", show_exits, 0, BASE, 0, 0},
        {NO, "maps", show_maps, 0, BASE, 0, 0},
        {NO, 0, 0, 0, 0, 0, 0}
};

command view_commands[] = {
	{NO, "view", 0, 0, 0, 0, 0}, 	/* Name of list */
        {NO, "motd", view_motd, 0, BASE, 0, 0},
	{NO, "rooms", command_rooms, 0, BASE, 0, 0}, 
        {NO, "time", time_of_day, 0, BASE, 0, 0},
        {NO, "traffic", network_traffic, 0, BASE, 0, 0},
	  
        {NO, 0, 0, 0, 0, 0, 0}
};

command main_command_list[] = {
	{YES, "\'", 	0, "c_say", BASE, c_ALS|c_NSP, 0},
	{YES, "\"", 	0, "c_say", BASE, c_ALS|c_NSP, 0},
	{YES, "`", 0, "c_say", BASE, c_ALS|c_NSP, 0},
	{YES, ";", 0, "c_say", BASE, c_ALS|c_NSP, 0},
	{YES, ":", 0, "c_say", BASE, c_ALS|c_NSP, 0},
	{YES, "~", 0, "c_say", BASE, c_ALS|c_NSP, 0},
        {YES, "!", 0, "redo_command", BASE, c_ALS|c_NSP, 0},
        {YES, ".", 0, "tell", BASE, c_ALS|c_NSP, 0},
        {YES, ",", 0, "remote", BASE, c_ALS|c_NSP, 0},
	{NO, 0, 0, 0, 0, 0, 0}, 
// A
        {YES, "a'", 0, "admin_say", ADMIN, c_NSP, 0},
        {YES, "a;", 0, "admin_emote", ADMIN, c_NSP, 0},
        {YES, "a~", 0, "admin_think", ADMIN, c_NSP, 0},
        {NO, 0, 0, 0, 0, 0, 0},
// B
        {NO, 0, 0, 0, 0, 0, 0},
// C
        {NO, "conmsg", c_conmsg, 0, RESIDENT, 0, 0},
	{NO, "commands", c_commands, 0, BASE, 0, 0}, 		
	{NO, 0, 0, 0, 0, 0, 0},
// D
        {NO, "d", go_down, 0, BASE, 0, 0},
        {NO, "description", c_description, 0, RESIDENT, 0, 0},
        {NO, "disconmsg",  c_disconmsg, 0, RESIDENT, 0, 0},
        {NO, 0, 0, 0, 0, 0, 0},
// E
        {NO, "e", go_east, 0, BASE, 0, 0},
        {NO, "email", c_email, 0, BASE, 0, 0},
	{YES, "emote", 0, "c_emote", BASE, 0, 0},
        {NO, "entermsg", c_entermsg, 0, RESIDENT, 0, 0},
        {YES, "examine", 0, "examine", BASE, 0, 0},
	{NO, 0, 0, 0, 0, 0, 0}, 
// F 
        {YES, "f", 0, "examine", BASE, 0, 0},
        {YES, "finger", 0, "examine", BASE, 0, 0},
        {NO, 0, 0, 0, 0, 0, 0},    
// G
        {YES, "gender", 0, "set_gender", RESIDENT, 0, 0},
        {YES, "grant", 0, "command_grant", ADMIN, 0, 0},
        {NO, 0, 0, 0, 0, 0, 0},
// H
        {NO, "help", command_help, 0, BASE, 0, 0},
        {NO, 0, 0, 0, 0, 0, 0},
// I
        {YES, "idle", 0, "c_idle", BASE, 0, 0},
        {NO, 0, 0, 0, 0, 0, 0},
// J
        {NO, "jump", command_jump, 0, SU, 0, 0},
        {NO, 0, 0, 0, 0, 0, 0},   
// K
        {NO, 0, 0, 0, 0, 0, 0},   
// L
	{NO, "l", command_look, 0, BASE, c_ALS, 0},
        {YES, "lockout", 0, "lockout", ADMIN, 0, 0},
	{NO, 0, 0, 0, 0, 0, 0}, 
// M
        {NO, 0, 0, 0, 0, 0, 0},
// N
        {NO, "n", go_north, 0, BASE, 0, 0},
        {YES, "newbies", 0, "close_to_newbies", SU, 0, 0},
        {NO, 0, 0, 0, 0, 0, 0},
// O 
        {NO, 0, 0, 0, 0, 0, 0},
// P
        {NO, "password", c_password,0,  BASE, 0, 0},
        {NO, "prompt", c_prompt, 0, BASE, 0, 0},
        {NO, 0, 0, 0, 0, 0, 0},   
// Q
	{NO, "quit", c_quit, 0, BASE, 0, 0},
	{NO, 0, 0, 0, 0, 0, 0}, 
// R
        {NO, "recap", c_recap, 0, RESIDENT, 0, 0},
        {NO, "reconmsg", c_reconmsg, 0, RESIDENT, 0, 0},
        {NO, "reload", reload, 0, LOWER_ADMIN, 0, 0},
        {YES, "remote", 0, "remote", BASE, 0, 0},
        {NO, "remove", 0, "command_remove", ADMIN, 0, 0},
        {YES, "ressies", 0, "ressies", LOWER_ADMIN, 0, 0},
        {NO, "room", c_null, 0, ADMIN, 0, room_commands},
        {NO, 0, 0, 0, 0, 0, 0},    
// S
        {YES, "s'", 0, "su_say", PSU, c_NSP, 0},
        {YES, "s;", 0, "su_emote", PSU, c_NSP, 0},
        {YES, "s~", 0, "su_think", PSU, c_NSP, 0},
        {NO, "s", go_south, 0, BASE, 0, 0},
        {NO, "save", save_player, 0, BASE, 0, 0},
	{YES, "say", 0, "c_say", BASE, 0, 0},
        {NO, "show", c_null, 0, BASE, 0, show_commands},
        {YES, "shout", 0, "do_shout", BASE, 0, 0},
	{YES, "shutdown", 0, "shutdown_system", LOWER_ADMIN, 0, 0},
        {NO, "splat", 0, "splat", SU, 0, 0},
	{NO, 0, 0, 0, 0, 0, 0}, 
// T
        {YES, "tell", 0, "tell", BASE, 0, 0},
        {NO, "term", c_terminal, 0, BASE, 0, 0},
        {NO, "title", c_title, 0, BASE, 0, 0},
        {NO, 0, 0, 0, 0, 0, 0},
// U
        {NO, "u", go_up, 0, BASE, 0, 0},
        {NO, "url", c_url, 0, RESIDENT, 0, 0},
        {NO, 0, 0, 0, 0, 0, 0},
// V
        {NO, "version", display_version, 0, BASE, 0, 0},
        {NO, "view", c_null, 0, BASE, 0, view_commands},
        {NO, 0, 0, 0, 0, 0, 0},  
// W
        {NO, "w", go_west, 0, BASE, 0, 0},
	{YES, "who", 0, "c_who", BASE, 0, 0},
	{NO, 0, 0, 0, 0, 0, 0}, 
// X
        {YES, "x", 0, "examine", BASE, 0, 0},
        {NO, 0, 0, 0, 0, 0, 0},
// Y
        {NO, 0, 0, 0, 0, 0, 0},
// Z
        {NO, 0, 0, 0, 0, 0, 0}   
     
};

#endif /* defined(COMMANDS_H) */
