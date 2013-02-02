
//////////////////////////////////////////////////////////////////
// File: proto.c                                                //
//////////////////////////////////////////////////////////////////
// Copyright (c) 1998.  Serpent Develeopment                    //
// Regents to Douglas Hanks.                                    //
//////////////////////////////////////////////////////////////////
// You may reach Serpent Development via email at:              //
// Douglas Hanks: ima@realms.org                                //
// Tom Malcom   : tom@ackphft.matsu.alaska.edu                  //
//////////////////////////////////////////////////////////////////

#if !defined(PROTO_H)
        #define PROTO_H 1
        #include "config.h"
        #include "structs.h"

/* buffer.c */
extern void              init_buffer(player *, unsigned long int);
extern void              deinit_buffer(player *);
extern int               buffer_add(player *, char *);
extern int               buffer_send(player *);

/* comms.c */
extern void              c_who(player *, char *);

/* help.c */
extern void              init_help_topics(void);

/* io.c */
extern int               player_load(player *);
extern void              save_player(player *, char *);

/* funcs.c */
extern void              su_wall(char *, ...);
extern void              ad_wall(char *, ...);
extern void              raw_wall(char *, ...);
extern char             *string_time(time_t);
extern char             *word_time(int);
extern char             *ping_string(player *p);
extern void              tell_player(player *, char *, ...);
extern char 		 asks(char *);
extern void 		 banner(player *, char *, char *, char);
extern char 		*bad_name(char *);
extern char 		*bad_password(player *, char *);
extern char 		*capfirst(char *);
extern void 		 config_report(player *, char *, ...);
extern void 		 dump_dfile(dfile);
extern char 		*end_string(char *);
extern int 		 get_bit(char *, bit *);
extern void 		 get_int(char **, int *);
extern char 		*get_key(int, bit *);
extern char 		*get_level_key(int);
extern char 		*get_line(char *, int *);
extern struct s_parser 	*get_parser(int);
extern void 		 get_string(char **, char *);
extern char 		*heshe(player *);
extern char 		*himher(player *);
extern char 		*hisher(player *);
extern int 		 is_admin(char *);
extern dfile             connect_msg, dis_msg, motd_msg, load_dfile(char *);
extern void 		 log(char *, char *);
extern char 		*logtime(void);
extern void 		 lower_case(char *);
extern char 		*next_space(char *);
extern char 		*num2word(int);
extern int 		 password_match(char *, char *);
extern void 		 put_int(char **, int);
extern void 		 put_string(char **, char *);
extern room 		*random_entrance(void);
extern char 		*trim_end(char *);
extern void 		 vlog(char *, char *, ...);
extern void 		 vwriteo(player *, player *, char *, ...);
extern void 		 vwritep(player *, char *, ...);
extern void 		 vwriter(player *, player *, room *, char *, ...);
extern void 		 writea(char *);
extern void 		 writeo(player *, player *, char *);

/* list.c */
extern void             add_list(player *, char *, int);
extern void             deconstruct_list(player *);

/* login.c */
extern void 		 login_screen(player *);
extern void              c_quit(player *, char *);
extern void              do_inform(char *, int);

/* main.c */
extern void 		 fatal(char *);
extern void              init_dfiles(void);
extern void              destroy_player(player *);

/* objects.c */
extern void              load_objects(void);
extern void              show_objects_in_room(room *);
/* parser.c */
extern int               init_command_modules(void);
extern void              run_command(player *, char *);
extern command 		 config_command_list[];
extern void 		 init_parser(void);
extern void 		 match_command(player *, char *);
extern struct s_parser 	 parser[];
extern command 		 print_command_list[];
extern void 		 subcommand(player *, char *, command *);
extern void 		 subcommand_list(player *, command *);


/* players.c */
extern void 		 c_save(player *, char *);
extern player 		*find_player_id(int);
extern player 		*find_player_name(char *);
extern void 		 link_player(player *);
extern player 		*malloc_player(void);
extern int 		 new_pid(void);
extern void              free_player(player *);
extern void              unlink_player(player *);

/* rooms.c */
extern room             *find_coordinates(int, int, int);
extern void              command_look(player *, char *);
extern void              load_public_rooms(void);
extern void 		 writer(player *, player *, room *, char *);
extern void              purge_player(player *);
extern void              setup_player(player *);
extern int               new_rid(void);
extern void              show_room_list(player *);
extern int               move_to(player *, room *);
extern room             *mainroom, *voidroom;
extern void              init_public_rooms(void);

/* set.c */
extern void              c_terminal(player *, char *);

/* signals.c */
extern void 		 init_signals(void);
extern void              stop_timer(void);
extern void              init_timer(void);

/* socket.c */
extern int               sock_send(struct sock *, char *);
extern void              set_echo(player *, int);
extern void              init_termtypes(void);
extern void              do_backspace(player *);
extern void              ping_timed(player *);
extern void              sock_listen(struct sock *, int);
extern void              ping_respond(player *);
extern void              writep(player *, char *);
extern void 		 close_socket(int);
extern void 		 flush_buffer(player *);
extern void              player_disconnect(player *);
extern int               player_input(player *);
#if DUALPLEX
 extern int              accept_connection(int);
#else
 extern int               accept_connection(void);
#endif
extern void              promptp(player *, char *);


/* vars.c */
extern void             *command_handle;
extern struct utsname    uts;
extern int               shutdown_count;
extern        tag_list   root_users[];
extern        tag_list   channels[];
extern        tag_list   defined_levels[];
extern        term_info  term_codes[];
extern struct system_info s_info; 
extern int               network_count;
extern int               SLOW_DNS;
extern int               uptime;
extern int               system_flags;
extern command 		**admin_commands,
			**operator_commands,
			**user_commands,
			**base_commands;
extern int 		 cache_depth;
extern int 		 channel;
extern struct s_config 	 config;
extern bit 		 config_flags[];
extern char 		*current_func;
extern char             *termtypes_msg;
extern player 		*current_player;
extern dfile 		 screen_one_msg, terminals_msg, compiled_msg;
extern room 		*dr_top[];
extern bit 		 levels[];
extern player 		*p_top;
extern char 		*reserved_names[];
extern room 		*r_top;
extern char 		*runtime_directories[];
extern char 		*stack;
extern int 		 timer;
extern int 		 users;

#endif
