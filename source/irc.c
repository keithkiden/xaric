#ident "$Id$"
/*
 * ircII: a new irc client.  I like it.  I hope you will too!
 *
 * Written By Michael Sandrof
 * Copyright(c) 1990 
 * See the COPYRIGHT file, or do a HELP IRCII COPYRIGHT 
 */

/*
 * INTERNAL_VERSION is the number that the special alias $V returns.
 * Make sure you are prepared for floods, pestilence, hordes of locusts,
 * and all sorts of HELL to break loose if you change this number.
 * Its format is actually YYYYMMDD, for the _release_ date of the
 * client..
 */
const char internal_version[] = "19971106";

#include "irc.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#ifdef USING_CURSES
#include <curses.h>
#endif
#include <stdarg.h>

#include "status.h"
#include "dcc.h"
#include "names.h"
#include "vars.h"
#include "input.h"
#include "alias.h"
#include "output.h"
#include "ircterm.h"
#include "exec.h"
#include "flood.h"
#include "screen.h"
#include "log.h"
#include "server.h"
#include "hook.h"
#include "keys.h"
#include "ircaux.h"
#include "commands.h"
#include "window.h"
#include "history.h"
#include "exec.h"
#include "notify.h"
#include "numbers.h"
#include "debug.h"
#include "newio.h"
#include "timer.h"
#include "whowas.h"
#include "misc.h"
#include "tcommand.h"

int irc_port = IRC_PORT,	/* port of ircd */
  strip_ansi_in_echo, current_on_hook = -1,	/* used in the send_text()
						 * routine */
  use_flow_control = USE_FLOW_CONTROL,	/* true: ^Q/^S used for flow
					 * cntl */
  current_numeric,		/* this is negative of the
				 * current numeric! */
  bflag = 1, key_pressed = 0, waiting_out = 0,	/* used by /WAIT command */
  waiting_in = 0,		/* used by /WAIT command */
  who_mask = 0;			/* keeps track of which /who
				 * switchs are set */

char zero[] = "0", one[] = "1", space[] = " ", on[] = "ON", off[] = "OFF";

char oper_command = 0;		/* true just after an oper() command is
				 * given.  Used to tell the difference
				 * between an incorrect password generated by
				 * an oper() command and one generated when
				 * connecting to a new server */
char global_all_off[2];		/* lame kludge to get around lameness */
struct in_addr MyHostAddr;	/* The local machine address */
struct in_addr LocalHostAddr;
char *LocalHostName = NULL;
extern char *last_away_nick;

extern int split_watch;
char empty_string[] = "";
char three_stars[] = "***";

char *invite_channel = NULL,	/* last channel of an INVITE */
 *ircrc_file = NULL,		/* full path .ircrc file */
 *my_path = NULL,		/* path to users home dir */
 *irc_path = NULL,		/* paths used by /load */
 *ircservers_file = NULL,	/* name  of server file */
  nickname[NICKNAME_LEN + 1],	/* users nickname */
  hostname[NAME_LEN + 1],	/* name of current host */
  realname[REALNAME_LEN + 1],	/* real name of user */
  username[NAME_LEN + 1],	/* usernameof user */
 *send_umode = NULL,		/* sent umode */
 *args_str = NULL,		/* list of command line args */
 *last_notify_nick = NULL,	/* last detected nickname */
 *who_name = NULL,		/* extra /who switch info */
 *who_file = NULL,		/* extra /who switch info */
 *who_server = NULL,		/* extra /who switch info */
 *who_host = NULL,		/* extra /who switch info */
 *who_nick = NULL,		/* extra /who switch info */
 *who_real = NULL,		/* extra /who switch info */
 *cannot_open = NULL,		/* extra /who switch info */
 *auto_str = NULL,		/* auto response str */
 *cut_buffer = NULL,		/* global cut_buffer */
 *line_thing = NULL;		/* show_numeric_str, say( ), put at begin of line */
 

int away_set = 0;		/* set if there is an away
				 * message anywhere */
int quick_startup = 0;		/* set if we ignore .ircrc */

#ifdef K_DEBUG
int cx_line = 0;
char cx_file[BIG_BUFFER_SIZE / 4];	/* debug file info */
char cx_function[BIG_BUFFER_SIZE / 4];
#endif

time_t idle_time = 0, start_time;
fd_set readables, writables;
int child_dead = 0;


/*
 * Signal handlers 
 */
static RETSIGTYPE cntl_c (int);
static RETSIGTYPE coredump (int);
static RETSIGTYPE sig_refresh_screen (int);
static RETSIGTYPE nothing (int);


static void parse_args (char **, int);

static volatile int cntl_c_hit = 0;

const char version[] = "Xaric";

static char switch_help[] =
"Usage: xaric [switches] [nickname] [server list] \n\
  The [nickname] can be at most 9 characters long\n\
  The [server list] is a whitespace separate list of server name\n\
  The [switches] may be any or all of the following\n\
   -H <hostname>\tuses the virtual hostname if possible\n\
   -b\t\tload .ircrc after connecting to a server\n\
   -p <port>\tdefault server connection port (usually 6667)\n\
   -f\t\tyour terminal uses flow controls (^S/^Q), so xaric shouldn't\n\
   -F\t\tyour terminal doesn't use flow control (default)\n\
   -q\t\tdoes not load ~/.ircrc\n\
   -r file\tload file as list of servers\n\
   -n nickname\tnickname to use\n\
   -a\t\tadds default servers and command line servers to server list\n\
   -x\t\truns xaric in \"debug\" mode\n\
   -v\t\ttells you about the client's version\n\
   -l <file>\tloads <file> in place of your .ircrc\n\
   -L <file>\tloads <file> in place of your .ircrc and expands $ expandos\n";







/* irc_exit: cleans up and leaves */
void
irc_exit (char *reason, char *formated)
{
	int old_window_display = window_display;

	do_hook (EXIT_LIST, "%s", reason);
	close_server (-1, reason);
	put_it (formated ? formated : reason);
	logger (curr_scr_win, NULL, 0);
	if (get_int_var (MSGLOG_VAR))
		log_toggle (0, NULL);

	clean_up_processes ();
	cursor_to_input ();	/* Needed so that ircII doesn't gobble
				 * the last line of the kill. */
	term_cr ();
	term_clear_to_eol ();
	term_reset ();

	/* Debugging sanity. */
	window_display = 0;
	set_lastlog_size (curr_scr_win, NULL, 0);
	set_history_size (curr_scr_win, NULL, 0);
	remove_channel (NULL, 0);
	window_display = old_window_display;
	clear_bindings ();
	clear_sets ();

	fprintf (stdout, "\r");
	fflush (stdout);
	exit (0);
}






/* sig_refresh_screen: the signal-callable version of refresh_screen */
static RETSIGTYPE 
sig_refresh_screen (int unused)
{
	refresh_screen (0, NULL);
}

/* irc_exit: cleans up and leaves */
static RETSIGTYPE 
irc_exit_old (int unused)
{
	irc_exit (get_string_var (SIGNOFF_REASON_VAR), NULL);
}

/* This is needed so that the fork()s we do to read compressed files dont
 * sit out there as zombies and chew up our fd's while we read more.
 */
static RETSIGTYPE 
child_reap (int sig)
{
	child_dead++;
}

static RETSIGTYPE 
nothing (int sig)
{
	/* nothing to do! */
}


static volatile int segv_recurse = 0;
/* sigsegv: something to handle segfaults in a nice way */
/* this needs to be changed to *NOT* use printf(). */
static RETSIGTYPE 
coredump (int sig)
{
	if (segv_recurse)
		_exit (1);
	segv_recurse = 1;

	printf ("\n\r\n\rXaric has been terminated by signal %i\n\r", sig);
	printf ("Please inform Laeos <laeos@ptw.com> of this\n\r");
	printf ("with as much detail as possible about what you were doing when it happened.\n\r");
	printf ("Please include the version of Xaric (%s) and type of system in the report.\n\r", irc_version);
	fflush (stdout);
	irc_exit ("Wow! A bug?", NULL);
}

/*
 * cntl_c: emergency exit.... if somehow everything else freezes up, hitting
 * ^C five times should kill the program. 
 */
static RETSIGTYPE 
cntl_c (int unused)
{

	if (cntl_c_hit++ >= 4)
		irc_exit ("User abort with 5 Ctrl-C's", NULL);
	else if (cntl_c_hit > 1)
		kill (getpid (), SIGALRM);
}


void 
display_name (int j)
{
	int i = strip_ansi_in_echo;
	strip_ansi_in_echo = 0;
	charset_ibmpc ();

	put_it (empty_string);
	put_it ("%s", convert_output_format ("%G-%R*%G- %RX %Ca r i c %G-%R*%G-", NULL));
	put_it ("[31mv%s brought to you by Laeos, Hawky, and Korndawg", irc_version);
	put_it (empty_string);

	charset_lat1 ();
	strip_ansi_in_echo = i;
}

/*
 * parse_args: parse command line arguments for irc, and sets all initial
 * flags, etc. 
 *
 * major rewrite 12/22/94 -jfn
 *
 *
 * Im going to break backwards compatability here:  I think that im 
 * safer in doing this becuase there are a lot less shell script with
 * the command line flags then there are ircII scripts with old commands/
 * syntax that would be a nasty thing to break..
 *
 * Sanity check:
 *   Supported flags: -b, -l, -v, -c, -p, -f, -F, -L, -a, -S, -z
 *   New (changed) flags: -s, -I, -i, -n
 *
 * Rules:
 *   Each flag must be included by a hyphen:  -lb <filename> is not the
 *              same as -l <filename> -b  any more...
 *   Each flag may or may not have a space between the flag and the argument.
 *              -lfoo  is the same as -l foo
 *   Anything surrounded by quotation marks is honored as one word.
 *   The -c, -p, -L, -l, -s, -z flags all take arguments.  If no arguments
 *              are given between the flag and the next flag, an error
 *              message is printed and the program is halted.
 *              Exception: the -s flag will be accepted without a argument.
 *              (ick: backwards compatability sucks. ;-)
 *   Arguments occuring after a flag that does not take an argument
 *              will be parsed in the following way: the first instance
 *              will be an assumed nickname, and the second instance will
 *              will be an assumed server. (some semblance of back compat.)
 *   The -bl sequence will emit a depreciated feature warning.
 *   The -n flag means "nickname"
 *
 */
static void
parse_args (char *argv[], int argc)
{

	int ac;
	int add_servers = 0;
	struct passwd *entry;
	char *ptr = NULL;
	struct hostent *hp;

	*nickname = 0;

	for (ac = 1; ac < argc; ac++)
	{
		if (argv[ac][0] == '-')
		{
			switch (argv[ac][1])
			{

			case 'v':	/* Output ircII version */
				{
					printf ("xaric version %s (%s)\n\r", irc_version, internal_version);
					exit (0);
				}

			case 'p':	/* Default port to use */
				{
					char *what = empty_string;

					if (argv[ac][2])
						what = &argv[ac][2];
					else if (argv[ac + 1] && argv[ac + 1][0] != '-')
					{
						what = argv[ac + 1];
						ac++;
					}
					else
					{
						fprintf (stderr, "Missing paramater after -p\n");
						exit (1);
					}
					irc_port = my_atol (what);
					break;
				}
			case 'f':	/* Use flow control */
				{
					use_flow_control = 1;
					if (argv[ac][2])
						fprintf (stderr, "Ignoring junk after -f\n");
					break;
				}

			case 'F':	/* dont use flow control */
				{
					use_flow_control = 0;
					if (argv[ac][2])
						fprintf (stderr, "Ignoring junk after -F\n");
					break;
				}

			case 'l':	/* Load some file instead of ~/.ircrc */
				{
					char *what = empty_string;

					if (argv[ac][2])
						what = &argv[ac][2];
					else if (argv[ac + 1] && argv[ac + 1][0] != '-')
					{
						what = argv[ac + 1];
						ac++;
					}
					else
					{
						fprintf (stderr, "Missing argument to -l\n");
						exit (1);
					}
					malloc_strcpy (&ircrc_file, what);
					break;
				}

			case 'L':	/* load and expand */
				{
					char *what = empty_string;

					if (argv[ac][2])
						what = &argv[ac][2];
					else if (argv[ac + 1] && argv[ac + 1][0] != '-')
					{
						what = argv[ac + 1];
						ac++;
					}
					else
					{
						fprintf (stderr, "Missing argument to -L\n");
						exit (1);
					}
					malloc_strcpy (&ircrc_file, what);
					malloc_strcat (&ircrc_file, " -");
					break;
				}

			case 'r':	/* Load list of servers from this file */
				{
					char *what = empty_string;

					if (argv[ac][2])
						what = &argv[ac][2];
					else if (argv[ac + 1] && argv[ac + 1][0] != '-')
					{
						what = argv[ac + 1];
						ac++;
					}
					else
						fprintf (stderr, "Missing argument to -r\n");

					if (*what)
					{
						add_servers = 1;
						malloc_strcpy (&ircservers_file, what);
					}
					break;
				}

			case 'a':	/* add server, not replace */
				{
					add_servers = 1;
					if (argv[ac][2])
						fprintf (stderr, "Ignoring junk after -a\n");
					break;
				}

			case 'q':	/* quick startup -- no .ircrc */
				{
					quick_startup = 1;
					if (argv[ac][2])
						fprintf (stderr, "Ignoring junk after -q\n");
					break;
				}

			case 'b':
				{
					bflag = 0;
					break;
				}

			case 'n':
				{
					char *what = empty_string;

					if (argv[ac][2])
						what = &(argv[ac][2]);
					else if (argv[ac + 1] && argv[ac + 1][0] != '-')
					{
						what = argv[ac + 1];
						ac++;
					}
					else
					{
						fprintf (stderr, "Missing argument for -n\n");
						exit (1);
					}
					strmcpy (nickname, what, NICKNAME_LEN);
					break;
				}

			case 'x':	/* set server debug */
				{
					x_debug = (unsigned long) 0xffffffff;
					if (argv[ac][2])
						fprintf (stderr, "Ignoring junk after -x\n");
					break;
				}

			case 'z':
				{
					char *what;
					if (argv[ac][2])
						what = &argv[ac][2];
					else if (argv[ac + 1] && argv[ac + 1][0] != '-')
					{
						what = argv[ac + 1];
						ac++;
					}
					else
						break;
					strmcpy (username, what, NAME_LEN);
					break;
				}
			case 'H':
				{
					char *what = empty_string;

					if (argv[ac][2])
						what = &argv[ac][2];
					else if (argv[ac + 1] && argv[ac + 1][0] != '-')
					{
						what = argv[ac + 1];
						ac++;
					}
					else
					{
						fprintf (stderr, "Missing argument to -H\n");
						exit (1);
					}
					LocalHostName = m_strdup (what);
					break;
				}
			case '\0':
				break;	/* ignore - alone */

			default:
				{
					fprintf (stderr, "Unknown flag: %s\n", argv[ac]);
					fprintf (stderr, "%s", switch_help);
					exit (1);
				}
			}	/* End of switch */
		}
		else
		{
			if (*nickname)
				build_server_list (argv[ac]);
			else
				strmcpy (nickname, argv[ac], NICKNAME_LEN);
		}
	}


	if (!ircservers_file)
		malloc_strcpy (&ircservers_file, ".ircservers");

	if (!ircrc_file && (ptr = getenv ("IRCRC")))
		malloc_strcpy (&ircrc_file, ptr);

	if (!*nickname && (ptr = getenv ("IRCNICK")))
		strmcpy (nickname, ptr, NICKNAME_LEN);
	if ((ptr = getenv ("IRCUMODE")))
		malloc_strcpy (&send_umode, ptr);

	if ((ptr = getenv ("IRCNAME")))
		strmcpy (realname, ptr, REALNAME_LEN);
	else if ((ptr = getenv ("NAME")))
		strmcpy (realname, ptr, REALNAME_LEN);

	if ((ptr = getenv ("IRCPATH")))
		malloc_strcpy (&irc_path, ptr);
	else
	{
		malloc_strcpy (&irc_path, ".:~/.irc:");
		printf ("%s\n", irc_lib);
		malloc_strcat (&irc_path, irc_lib);
		malloc_strcat (&irc_path, "script");
	}

	set_string_var (LOAD_PATH_VAR, irc_path);
	new_free (&irc_path);

	if ((entry = getpwuid (getuid ())))
	{
		if (!*realname && entry->pw_gecos && *(entry->pw_gecos))
		{
#ifdef GECOS_DELIMITER
			if ((ptr = index (entry->pw_gecos, GECOS_DELIMITER)))
				*ptr = (char) 0;
#endif
			if ((ptr = strchr (entry->pw_gecos, '&')) == NULL)
				strmcpy (realname, entry->pw_gecos, REALNAME_LEN);
			else
			{
				int len = ptr - entry->pw_gecos;

				if (len < REALNAME_LEN && *(entry->pw_name))
				{
					char *q = realname + len;

					strmcpy (realname, entry->pw_gecos, len);
					strmcat (realname, entry->pw_name, REALNAME_LEN);
					strmcat (realname, ptr + 1, REALNAME_LEN);
					if (islower (*q) && (q == realname || isspace (*(q - 1))))
						*q = toupper (*q);
				}
				else
					strmcpy (realname, entry->pw_gecos, REALNAME_LEN);
			}
		}
		if (entry->pw_name && *(entry->pw_name) && !*username)
			strmcpy (username, entry->pw_name, NAME_LEN);
		if (entry->pw_dir && *(entry->pw_dir))
			malloc_strcpy (&my_path, entry->pw_dir);
	}

	if ((ptr = getenv ("HOME")))
		malloc_strcpy (&my_path, ptr);
	else if (!my_path || !*my_path)
		malloc_strcpy (&my_path, "/");
	if (!*realname)
		strmcpy (realname, "*Unknown*", REALNAME_LEN);

	/*
	 * Yes... this is EXACTLY what you think it is.  And if you don't know..
	 * then I'm not about to tell you!           -- Jake [WinterHawk] Khuon
	 */
	if ((ptr = getenv ("IRCUSER")))
		strmcpy (username, ptr, NAME_LEN);
	else if ((ptr = getenv ("USER")))
		strmcpy (username, ptr, NAME_LEN);
	else if (!*username)
	{
		strmcpy (username, "Unknown", NAME_LEN);

	}
	if (!LocalHostName && ((ptr = getenv ("IRC_HOST")) || (ptr = getenv ("IRCHOST"))))
		LocalHostName = m_strdup (ptr);

	if ((gethostname (hostname, sizeof (hostname))))
		if (!LocalHostName)
			exit (1);

	if (LocalHostName)
	{
		printf ("Your hostname appears to be [%s]\n", LocalHostName);
		memset ((void *) &LocalHostAddr, 0, sizeof (LocalHostAddr));
		if ((hp = gethostbyname (LocalHostName)))
			memcpy ((void *) &LocalHostAddr, hp->h_addr, sizeof (LocalHostAddr));
	}
	else
	{
		if ((hp = gethostbyname (hostname)))
			memcpy ((char *) &MyHostAddr, hp->h_addr, sizeof (MyHostAddr));
	}

	if (!nickname || !*nickname)
		strmcpy (nickname, username, sizeof (nickname));

	if (!check_nickname (nickname))
	{
		fprintf (stderr, "Illegal nickname %s\n", nickname);
		fprintf (stderr, "Please restart IRC II with a valid nickname\n");
		exit (1);
	}
	if (ircrc_file == NULL)
	{
		ircrc_file = (char *) new_malloc (strlen (my_path) + strlen (IRCRC_NAME) + 10);
		strcpy (ircrc_file, my_path);
		strcat (ircrc_file, "/");
		strcat (ircrc_file, IRCRC_NAME);
	}

	if (read_server_file (ircservers_file) || (server_list_size () == 0))
	{
		if ((ptr = getenv ("IRCPORT")))
			irc_port = my_atol (ptr);

		if ((ptr = getenv ("IRCSERVER")))
			build_server_list (ptr);
		ptr = NULL;

#ifdef DEFAULT_SERVER
		malloc_strcpy (&ptr, DEFAULT_SERVER);
		build_server_list (ptr);
		new_free (&ptr);
#else
		ircpanic ("DEFAULT_SERVER not defined -- no server list");
#endif
		from_server = -1;

	}


	return;
}



/* new irc_io modularized stuff */

/* 
 * GetLineStruct is what is "under" your current input line, and the function
 * we're supposed to call when you press return.  This is different from
 * AddWaitPrompt which does functionally the same thing but doesnt cause
 * recursive calls to io.
 */
struct GetLineStruct
{
	int done;
	void (*func) (char, char *);
	char *saved_input;
	char *saved_prompt;
	int recursive_call;
	struct GetLineStruct *prev;
	struct GetLineStruct *next;
};
typedef struct GetLineStruct GetLine;
GetLine *GetLineStack = NULL;

/* when you press return, you call this. */
extern void 
get_line_return (char unused, char *not_used)
{
	GetLine *stuff;

	/* get the last item on the stack */
	if ((stuff = GetLineStack) == NULL)
		return;

	/* 
	   If we're NOT the main() call, then undo all that we 
	   messed up coming in. 
	   If stuff->done gets set to 1 when recursive_call is 
	   zero, then something is VERY wrong.
	   We can set stuff->prev->next to null because the call
	   to get_line() holds a pointer to stuff, so when it
	   unrecurses, it will free it.
	 */
	if (stuff->func)
	{
		not_used = NULL;
		(stuff->func) (unused, not_used);
	}
	if (stuff->recursive_call)
	{
		stuff->done = 1;
		set_input (stuff->saved_input);
		set_input_prompt (curr_scr_win, stuff->saved_prompt, 0);
		new_free (&(stuff->saved_input));
		new_free (&(stuff->saved_prompt));
		stuff->next->prev = NULL;
		GetLineStack = stuff->next;
	}

	update_input (UPDATE_ALL);

	/* We cant delete stuff here becuase the get_line function
	 * still needs to look at stuff->done.  So we let it delete
	 * the items off the list.  But we removed it from the list,
	 * so we wont accidentally use it later.
	 */
	return;
}

/* This is a wrapper for io().  Only two functions at any time are allowed
 * to call it, and main() is one of those two.  When you call it, you have
 * the option to change the input prompt and the input buffer.  You also
 * give it a function to call when it gets a return.  Only main() is 
 * allowed to call it with an new_input of -1, which tells it that it is
 * at the lowest level of parsing, by which i mean that noone is waiting
 * for anything, since there is no recursion going on.
 */
void 
get_line (char *prompt, int new_input, void (*func) (char, char *))
{
	GetLine *stuff;

	if (GetLineStack && new_input == -1)
		ircpanic ("Illegal call to get_line\n");

	/* initialize the new item. */
	stuff = (GetLine *) new_malloc (sizeof (GetLine));
	stuff->done = 0;
	stuff->func = func;
	stuff->recursive_call = (new_input == -1) ? 0 : 1;
	stuff->saved_input = NULL;
	stuff->saved_prompt = NULL;
	stuff->prev = NULL;
	stuff->next = NULL;
	malloc_strcpy (&(stuff->saved_input), get_input ());
	malloc_strcpy (&(stuff->saved_prompt), get_input_prompt ());

	/* put it on the stack */
	if (GetLineStack)
	{
		stuff->next = GetLineStack;
		GetLineStack->prev = stuff;
	}
	GetLineStack = stuff;

	/* if its a global call, get the input prompt */
	if (new_input == -1)
		set_input_prompt (curr_scr_win, get_string_var (INPUT_PROMPT_VAR), 0);
	else
		set_input_prompt (curr_scr_win, prompt, 0);
	set_input (empty_string);

	/* ok.  we call io() until the user presses return, ending 
	 * the input line.  get_line_return will then set get_line_done
	 * to one, and we will stop getting characters and drop out.
	 * get_line_done NEVER sets this to one if we are in our call
	 * from main().  NEVER.
	 */
	while (!stuff->done)
		io ("get line");

	if (new_input == -1)
		ircpanic ("get_line: input == -1 is illegal value");

	/* By the time we get here, stuff->done has been set to 1,
	 * which means that get_line_return has already freed the
	 * interesting items in stuff and removed it from the list.
	 * Noone but us has a pointer to it, so we free it here.
	 */
	new_free (&stuff->saved_input);
	new_free (&stuff->saved_prompt);
	new_free ((char **) &stuff);
}

/* This simply waits for a key to be pressed before it unrecurses.
 * It doesnt do anyting in particular with that key (it will go to 
 * the input buffer, actually)
 */
char 
get_a_char (void)
{
	key_pressed = 0;
	while (!key_pressed)
		io ("get a char");
	update_input (UPDATE_ALL);
	return key_pressed;
}

/* 
 * io() is a ONE TIME THROUGH loop!  It simply does ONE check on the
 * file descriptors, and if there is nothing waiting, it will time
 * out and drop out.  It does everything as far as checking for exec,
 * dcc, ttys, notify, the whole ball o wax, but it does NOT iterate!
 * 
 * You should usually NOT call io() unless you are specifically waiting
 * for something from a file descriptor.  It doesnt look like bad things
 * will happen if you call this elsewhere, but its long time behavior has
 * not been observed.  It *does* however, appear to be much more reliable
 * then the old irc_io, and i even know how this works. >;-)
 */
extern void set_screens (fd_set *, fd_set *);

void 
io (const char *what)
{
	static int first_time = 1, level = 0;
	static struct timeval cursor_timeout, clock_timeout, right_away,
	  timer, *timeptr = NULL;
	int hold_over;
	fd_set rd, wd;
	static int old_level = 0;
	Screen *screen, *old_current_screen = current_screen;
	static const char *caller[51] =
	{NULL};			/* XXXX */
	static int last_warn = 0;
	time_t now = time (NULL);

	level++;

	if (x_debug & DEBUG_WAITS)
	{
		if (level != old_level)
		{
			yell ("Moving from io level [%d] to level [%d] from [%s]", old_level, level, what);
			old_level = level;
		}
	}


	if (level && (level - last_warn == 5))
	{
		last_warn = level;
		yell ("io's nesting level is [%d],  [%s]<-[%s]<-[%s]<-[%s]<-[%s]<-[%s]", level, what, caller[level - 1], caller[level - 2], caller[level - 3], caller[level - 4]);
		if (level % 50 == 0)
			ircpanic ("Ahoy there matey!  Abandon ship!");
		return;
	}
	else if (level && (last_warn - level == 5))
		last_warn -= 5;

	caller[level] = what;

	/* first time we run this function, set up the timeouts */
	if (first_time)
	{
		first_time = 0;

		/* time before cursor jumps from display area to input line */
		cursor_timeout.tv_usec = 0L;
		cursor_timeout.tv_sec = 1L;

		/*
		 * time delay for updating of internal clock
		 *
		 * Instead of looking every 15 seconds and seeing if
		 * the clock has changed, we now figure out how much
		 * time there is to the next clock change and then wait
		 * until then.  There is a small performance penalty 
		 * in actually calculating when the next minute will tick, 
		 * but that will be offset by the fact that we will only
		 * call select() once a minute instead of 4 times.
		 */
		clock_timeout.tv_usec = 0L;

		right_away.tv_usec = 0L;
		right_away.tv_sec = 0L;

		timer.tv_usec = 0L;
	}

	/* SET UP TIMEOUTS USED IN SELECTING */
/*      clock_timeout.tv_sec = time_to_next_minute(); */

	rd = readables;
	wd = writables;

	FD_ZERO (&wd);
	FD_ZERO (&rd);

	set_screens (&rd, &wd);
	set_dcc_bits (&rd, &wd);
	set_server_bits (&rd, &wd);
	set_process_bits (&rd);
	set_socket_read (&rd, &wd);

	clock_timeout.tv_sec = (60 - now % 60) + (now - idle_time);

	if (!timeptr)
		timeptr = &clock_timeout;
	timer.tv_sec = TimerTimeout ();
	if (timer.tv_sec <= timeptr->tv_sec)
		timeptr = &timer;
#if 0
	if ((hold_over = unhold_windows ()) != 0)
		timeptr = &right_away;
#else
	hold_over = 0;
#endif

	/* go ahead and wait for some data to come in */
	switch (new_select (&rd, &wd, timeptr))
	{
	case 0:
		break;
	case -1:
		{
			/* if we just got a sigint */
			if (cntl_c_hit)
			{
				key_pressed = 3;
				edit_char ('\003');
				cntl_c_hit = 0;
			}
			else if (errno != EINTR && errno > 0)
				yell ("Select failed with [%s]", strerror (errno));
			break;

		}

		/* we got something on one of the descriptors */
	default:
		{
			set_current_screen (last_input_screen);
			dcc_check (&rd, &wd);
			do_server (&rd, &wd);
			do_processes (&rd);
			do_screens (&rd);
			dcc_check_idle ();
			scan_sockets (&rd, &wd);
			set_current_screen (old_current_screen);
			break;
		}
	}
	ExecuteTimers ();
	while (child_dead)
	{
		check_wait_status (-1);
		child_dead--;
	}

	if (!hold_over)
		cursor_to_input ();
	timeptr = &clock_timeout;

	for (screen = screen_list; screen; screen = screen->next)
		if (screen->alive && is_cursor_in_display (screen))
			timeptr = &cursor_timeout;

	if (get_int_var (LLOOK_VAR) && from_server > -1 && !server_list[from_server].link_look)
	{
		if (time (NULL) - server_list[from_server].link_look_time > get_int_var (LLOOK_DELAY_VAR))
		{
			server_list[from_server].link_look++;
			my_send_to_server (from_server, "LINKS");
			server_list[from_server].link_look_time = time (NULL);
		}
	}
	if (update_clock (0))
	{
		do_notify ();
		clean_whowas_chan_list ();
		clean_whowas_list ();
		clean_flood_list ();
		if (get_int_var (CLOCK_VAR))
		{
			update_all_status (curr_scr_win, NULL, 0);
			cursor_to_input ();
		}
		check_server_connect (from_server);
	}

	/* (set in term.c) -- we should redraw the screen here */
	if (need_redraw)
		refresh_screen (0, NULL);

	caller[level] = NULL;
	level--;
	return;
}

int 
main (int argc, char *argv[], char *envp[])
{
	srand ((unsigned) time (NULL));
	time (&start_time);
	time (&idle_time);


#ifdef K_DEBUG
	*cx_file = 0;
	cx_line = 0;
	*cx_function = 0;
#endif

	if (isatty (0))
	{
		printf ("Process [%d] connected to tty [%s]\n", getpid (), ttyname (0));
	}
	else
	{
		fprintf (stderr, "Woops I need a tty!\n");
		exit (1);
	}

	parse_args (argv, argc);

	FD_ZERO (&readables);
	FD_ZERO (&writables);

	my_signal (SIGSEGV, coredump, 0);
	my_signal (SIGBUS, coredump, 0);
	my_signal (SIGQUIT, SIG_IGN, 0);
	my_signal (SIGHUP, irc_exit_old, 0);
	my_signal (SIGTERM, irc_exit_old, 0);
	my_signal (SIGPIPE, SIG_IGN, 0);
	my_signal (SIGINT, cntl_c, 0);
	my_signal (SIGCHLD, child_reap, 0);
	my_signal (SIGALRM, nothing, 0);

	if (init_screen ())
	{
		printf ("Woops! Couldn't init the terminal\n");
		exit (1);
	}

	my_signal (SIGCONT, term_cont, 0);
	my_signal (SIGWINCH, sig_refresh_screen, 0);

	init_variables ();
	init_keys_1 ();
	init_commands ();

	build_status (curr_scr_win, NULL, 0);
	update_input (UPDATE_ALL);


	global_all_off[0] = ALL_OFF;
	global_all_off[1] = '\0';

	display_name (-1);
	if (bflag)
		load_scripts ();

	get_connected (0);

	set_input (empty_string);
	get_line (NULL, -1, send_line);
	ircpanic ("get_line() returned");
	return (-((int) 0xdead));
}
