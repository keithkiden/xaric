#include "irc.h"
#include "ircaux.h"
#include "screen.h"
#include "output.h"
#include "misc.h"
#include "vars.h"
#include "fset.h"
#include "format.h"
#include "alist.h"
#include "tcommand.h"

FsetArray fset_array[] =
{
	{"FORMAT_381", NULL},
	{"FORMAT_391", NULL},
	{"FORMAT_443", NULL},
	{"FORMAT_471", NULL},
	{"FORMAT_473", NULL},
	{"FORMAT_474", NULL},
	{"FORMAT_475", NULL},
	{"FORMAT_476", NULL},

	{"FORMAT_ACTION", NULL},
	{"FORMAT_ACTION_OTHER", NULL},
	{"FORMAT_ALIAS", NULL},
	{"FORMAT_ASSIGN", NULL},
	{"FORMAT_AWAY", NULL},
	{"FORMAT_BACK", NULL},
	{"FORMAT_BANS", NULL},
	{"FORMAT_BANS_HEADER", NULL},
	{"FORMAT_BWALL", NULL},

	{"FORMAT_CHANNEL_SIGNOFF", NULL},

	{"FORMAT_CONNECT", NULL},
	{"FORMAT_CTCP", NULL},
	{"FORMAT_CTCP_REPLY", NULL},
	{"FORMAT_CTCP_UNKNOWN", NULL},
	{"FORMAT_DCC", NULL},
	{"FORMAT_DCC_CHAT", NULL},
	{"FORMAT_DCC_CONNECT", NULL},
	{"FORMAT_DCC_ERROR", NULL},
	{"FORMAT_DCC_LOST", NULL},
	{"FORMAT_DCC_REQUEST", NULL},
	{"FORMAT_DESYNC", NULL},

	{"FORMAT_DISCONNECT", NULL},
	{"FORMAT_ENCRYPTED_NOTICE", NULL},
	{"FORMAT_ENCRYPTED_PRIVMSG", NULL},
	{"FORMAT_FLOOD", NULL},
	{"FORMAT_HOOK", NULL},

	{"FORMAT_IGNORE_INVITE", NULL},
	{"FORMAT_IGNORE_MSG", NULL},
	{"FORMAT_IGNORE_MSG_AWAY", NULL},
	{"FORMAT_IGNORE_NOTICE", NULL},
	{"FORMAT_IGNORE_WALL", NULL},

	{"FORMAT_INVITE", NULL},
	{"FORMAT_INVITE_USER", NULL},
	{"FORMAT_JOIN", NULL},
	{"FORMAT_KICK", NULL},
	{"FORMAT_KICK_USER", NULL},
	{"FORMAT_KILL", NULL},
	{"FORMAT_LEAVE", NULL},
	{"FORMAT_LINKS", NULL},
	{"FORMAT_LIST", NULL},
	{"FORMAT_MODE", NULL},
	{"FORMAT_MODE_CHANNEL", NULL},
	{"FORMAT_MSG", NULL},
	{"FORMAT_MSGLOG", NULL},
	{"FORMAT_MSG_GROUP", NULL},
	{"FORMAT_NAMES", NULL},
	{"FORMAT_NAMES_BANNER", NULL},

	{"FORMAT_NAMES_FOOTER", NULL},
	{"FORMAT_NAMES_IRCOP", NULL},
	{"FORMAT_NAMES_NICKCOLOR", NULL},
	{"FORMAT_NAMES_NONOP", NULL},
	{"FORMAT_NAMES_OP", NULL},
	{"FORMAT_NAMES_OPCOLOR", NULL},
	{"FORMAT_NAMES_VOICE", NULL},
	{"FORMAT_NAMES_VOICECOLOR", NULL},

	{"FORMAT_NETADD", NULL},
	{"FORMAT_NETJOIN", NULL},
	{"FORMAT_NETSPLIT", NULL},
	{"FORMAT_NETSPLIT_HEADER", NULL},
	{"FORMAT_NICKNAME", NULL},
	{"FORMAT_NICKNAME_OTHER", NULL},
	{"FORMAT_NICKNAME_USER", NULL},
	{"FORMAT_NICK_AUTO", NULL},
	{"FORMAT_NICK_COMP", NULL},
	{"FORMAT_NICK_MSG", NULL},
	{"FORMAT_NONICK", NULL},
	{"FORMAT_NOTE", NULL},
	{"FORMAT_NOTICE", NULL},
	{"FORMAT_NOTIFY_OFF", NULL},
	{"FORMAT_NOTIFY_ON", NULL},
	{"FORMAT_NOTIFY_SIGNOFF", NULL},
	{"FORMAT_NOTIFY_SIGNOFF_UH", NULL},
	{"FORMAT_NOTIFY_SIGNON", NULL},
	{"FORMAT_NOTIFY_SIGNON_UH", NULL},
	{"FORMAT_OPER", NULL},
	{"FORMAT_PUBLIC", NULL},
	{"FORMAT_PUBLIC_AR", NULL},
	{"FORMAT_PUBLIC_MSG", NULL},
	{"FORMAT_PUBLIC_MSG_AR", NULL},
	{"FORMAT_PUBLIC_NOTICE", NULL},
	{"FORMAT_PUBLIC_NOTICE_AR", NULL},
	{"FORMAT_PUBLIC_OTHER", NULL},
	{"FORMAT_PUBLIC_OTHER_AR", NULL},
	{"FORMAT_SEND_ACTION", NULL},
	{"FORMAT_SEND_ACTION_OTHER", NULL},
	{"FORMAT_SEND_AWAY", NULL},
	{"FORMAT_SEND_CTCP", NULL},
	{"FORMAT_SEND_DCC_CHAT", NULL},
	{"FORMAT_SEND_MSG", NULL},
	{"FORMAT_SEND_NOTICE", NULL},
	{"FORMAT_SEND_PUBLIC", NULL},
	{"FORMAT_SEND_PUBLIC_OTHER", NULL},
	{"FORMAT_SERVER", NULL},
	{"FORMAT_SERVER_MSG1", NULL},
	{"FORMAT_SERVER_MSG1_FROM", NULL},
	{"FORMAT_SERVER_MSG2", NULL},
	{"FORMAT_SERVER_MSG2_FROM", NULL},
	{"FORMAT_SERVER_NOTICE", NULL},

	{"FORMAT_SET", NULL},
	{"FORMAT_SET_NOVALUE", NULL},
	{"FORMAT_SIGNOFF", NULL},
	{"FORMAT_SILENCE", NULL},
	{"FORMAT_SMODE", NULL},
	{"FORMAT_STATUS", NULL},
	{"FORMAT_STATUS2", NULL},
	{"FORMAT_TIMER", NULL},
	{"FORMAT_TOPIC", NULL},
	{"FORMAT_TOPIC_CHANGE", NULL},
	{"FORMAT_TOPIC_CHANGE_HEADER", NULL},
	{"FORMAT_TOPIC_SETBY", NULL},
	{"FORMAT_TOPIC_UNSET", NULL},
	{"FORMAT_TRACE_OPER", NULL},
	{"FORMAT_TRACE_SERVER", NULL},
	{"FORMAT_TRACE_USER", NULL},
	{"FORMAT_USAGE", NULL},
	{"FORMAT_USERMODE", NULL},
	{"FORMAT_USERS", NULL},
	{"FORMAT_USERS_HEADER", NULL},
	{"FORMAT_USERS_USER", NULL},
	{"FORMAT_VERSION", NULL},
	{"FORMAT_WALL", NULL},
	{"FORMAT_WALLOP", NULL},
	{"FORMAT_WALL_AR", NULL},
	{"FORMAT_WHO", NULL},
	{"FORMAT_WHOIS_AWAY", NULL},
	{"FORMAT_WHOIS_CHANNELS", NULL},
	{"FORMAT_WHOIS_FOOTER", NULL},
	{"FORMAT_WHOIS_HEADER", NULL},
	{"FORMAT_WHOIS_IDLE", NULL},
	{"FORMAT_WHOIS_NAME", NULL},
	{"FORMAT_WHOIS_NICK", NULL},
	{"FORMAT_WHOIS_OPER", NULL},
	{"FORMAT_WHOIS_SERVER", NULL},
	{"FORMAT_WHOIS_SIGNON", NULL},
	{"FORMAT_WHOLEFT_FOOTER", NULL},
	{"FORMAT_WHOLEFT_HEADER", NULL},
	{"FORMAT_WHOLEFT_USER", NULL},
	{"FORMAT_WHOWAS_HEADER", NULL},
	{"FORMAT_WHOWAS_NICK", NULL},
	{"FORMAT_WIDELIST", NULL},
	{"FORMAT_WINDOW_SET", NULL},
	{NULL, NULL}
};

char *
get_fset_var (enum FSET_TYPES var)
{
	return (fset_array[var].string);
}

void 
set_fset_var (enum FSET_TYPES var, char *value)
{
	if (value && *value)
		malloc_strcpy (&(fset_array[var].string), value);
	else
		new_free (&(fset_array[var].string));
}

static int 
find_fset_variable (FsetArray * array, char *org_name, int *cnt)
{
	FsetArray *v, *first;
	int len, var_index;
	char *name = NULL;

	malloc_strcpy (&name, org_name);
	upper (name);
	len = strlen (name);
	var_index = 0;
	for (first = array; first->name; first++, var_index++)
	{
		if (strncmp (name, first->name, len) == 0)
		{
			*cnt = 1;
			break;
		}
	}
	if (first->name)
	{
		if (strlen (first->name) != len)
		{
			v = first;
			for (v++; v->name; v++, (*cnt)++)
			{
				if (strncmp (name, v->name, len) != 0)
					break;
			}
		}
		new_free (&name);
		return (var_index);
	}
	else
	{
		*cnt = 0;
		new_free (&name);
		return (-1);
	}
}


static void 
set_fset_var_value (int var_index, char *value)
{
	FsetArray *var;
	var = &(fset_array[var_index]);

	if (value)
	{
		if (*value)
			malloc_strcpy (&(var->string), value);
		else
		{
			put_it ("%s", convert_output_format (get_fset_var (FORMAT_SET_FSET), "%s %s", var->name, var->string ? var->string : empty_string));
			return;
		}
	}
	else
		new_free (&(var->string));
	say ("Value of %s set to %s", var->name, var->string ? var->string : "<EMPTY>");
}

static inline void 
fset_variable_casedef (char *name, int cnt, int var_index, char *args)
{
	for (cnt += var_index; var_index < cnt; var_index++)
		set_fset_var_value (var_index, args);
}

static inline void 
fset_variable_noargs (char *name)
{
	int var_index = 0;
	for (var_index = 0; var_index < NUMBER_OF_FSET; var_index++)
		set_fset_var_value (var_index, empty_string);
}


void
cmd_fset (struct command *cmd, char *args)
{
	char *var;
	char *name = NULL;
	int cnt, var_index;

	if ((var = next_arg (args, &args)) != NULL)
	{
		if (*var == '-')
		{
			var++;
			args = NULL;
		}
		var_index = find_fset_variable (fset_array, var, &cnt);
		switch (cnt)
		{
		case 0:
			say ("No such variable \"%s\"", var);
			return;
		case 1:
			set_fset_var_value (var_index, args);
			return;
		default:
			say ("%s is ambiguous", var);
			fset_variable_casedef (name, cnt, var_index, empty_string);
			return;
		}
	}
	fset_variable_noargs (name);
}


void 
create_fsets (void)
{
	set_fset_var (FORMAT_381_FSET, DEFAULT_FMT_381_FSET);
	set_fset_var (FORMAT_391_FSET, DEFAULT_FMT_391_FSET);
	set_fset_var (FORMAT_443_FSET, DEFAULT_FMT_443_FSET);

	set_fset_var (FORMAT_471_FSET, DEFAULT_FMT_471_FSET);
	set_fset_var (FORMAT_473_FSET, DEFAULT_FMT_473_FSET);
	set_fset_var (FORMAT_474_FSET, DEFAULT_FMT_474_FSET);
	set_fset_var (FORMAT_475_FSET, DEFAULT_FMT_475_FSET);
	set_fset_var (FORMAT_476_FSET, DEFAULT_FMT_476_FSET);

	set_fset_var (FORMAT_ACTION_FSET, DEFAULT_FMT_ACTION_FSET);
	set_fset_var (FORMAT_ACTION_OTHER_FSET, DEFAULT_FMT_ACTION_OTHER_FSET);

	set_fset_var (FORMAT_ALIAS_FSET, DEFAULT_FMT_ALIAS_FSET);
	set_fset_var (FORMAT_ASSIGN_FSET, DEFAULT_FMT_ASSIGN_FSET);
	set_fset_var (FORMAT_AWAY_FSET, DEFAULT_FMT_AWAY_FSET);
	set_fset_var (FORMAT_BACK_FSET, DEFAULT_FMT_BACK_FSET);
	set_fset_var (FORMAT_BANS_HEADER_FSET, DEFAULT_FMT_BANS_HEADER_FSET);
	set_fset_var (FORMAT_BANS_FSET, DEFAULT_FMT_BANS_FSET);

	set_fset_var (FORMAT_BWALL_FSET, DEFAULT_FMT_BWALL_FSET);

	set_fset_var (FORMAT_CHANNEL_SIGNOFF_FSET, DEFAULT_FMT_CHANNEL_SIGNOFF_FSET);
	set_fset_var (FORMAT_CONNECT_FSET, DEFAULT_FMT_CONNECT_FSET);


	set_fset_var (FORMAT_CTCP_FSET, DEFAULT_FMT_CTCP_FSET);
	set_fset_var (FORMAT_CTCP_UNKNOWN_FSET, DEFAULT_FMT_CTCP_UNKNOWN_FSET);
	set_fset_var (FORMAT_CTCP_REPLY_FSET, DEFAULT_FMT_CTCP_REPLY_FSET);


	set_fset_var (FORMAT_DCC_CHAT_FSET, DEFAULT_FMT_DCC_CHAT_FSET);
	set_fset_var (FORMAT_DCC_CONNECT_FSET, DEFAULT_FMT_DCC_CONNECT_FSET);
	set_fset_var (FORMAT_DCC_ERROR_FSET, DEFAULT_FMT_DCC_ERROR_FSET);
	set_fset_var (FORMAT_DCC_LOST_FSET, DEFAULT_FMT_DCC_LOST_FSET);
	set_fset_var (FORMAT_DCC_REQUEST_FSET, DEFAULT_FMT_DCC_REQUEST_FSET);
	set_fset_var (FORMAT_DESYNC_FSET, DEFAULT_FMT_DESYNC_FSET);
	set_fset_var (FORMAT_DISCONNECT_FSET, DEFAULT_FMT_DISCONNECT_FSET);
	set_fset_var (FORMAT_ENCRYPTED_NOTICE_FSET, DEFAULT_FMT_ENCRYPTED_NOTICE_FSET);
	set_fset_var (FORMAT_ENCRYPTED_PRIVMSG_FSET, DEFAULT_FMT_ENCRYPTED_PRIVMSG_FSET);
	set_fset_var (FORMAT_FLOOD_FSET, DEFAULT_FMT_FLOOD_FSET);
	set_fset_var (FORMAT_HOOK_FSET, DEFAULT_FMT_HOOK_FSET);
	set_fset_var (FORMAT_INVITE_FSET, DEFAULT_FMT_INVITE_FSET);
	set_fset_var (FORMAT_INVITE_USER_FSET, DEFAULT_FMT_INVITE_USER_FSET);
	set_fset_var (FORMAT_JOIN_FSET, DEFAULT_FMT_JOIN_FSET);
	set_fset_var (FORMAT_KICK_FSET, DEFAULT_FMT_KICK_FSET);
	set_fset_var (FORMAT_KICK_USER_FSET, DEFAULT_FMT_KICK_USER_FSET);
	set_fset_var (FORMAT_KILL_FSET, DEFAULT_FMT_KILL_FSET);
	set_fset_var (FORMAT_LEAVE_FSET, DEFAULT_FMT_LEAVE_FSET);
	set_fset_var (FORMAT_LINKS_FSET, DEFAULT_FMT_LINKS_FSET);
	set_fset_var (FORMAT_LIST_FSET, DEFAULT_FMT_LIST_FSET);

	set_fset_var (FORMAT_MSGLOG_FSET, DEFAULT_FMT_MSGLOG_FSET);

	set_fset_var (FORMAT_MODE_FSET, DEFAULT_FMT_MODE_FSET);
	set_fset_var (FORMAT_SMODE_FSET, DEFAULT_FMT_SMODE_FSET);
	set_fset_var (FORMAT_MODE_CHANNEL_FSET, DEFAULT_FMT_MODE_CHANNEL_FSET);

	set_fset_var (FORMAT_MSG_FSET, DEFAULT_FMT_MSG_FSET);

	set_fset_var (FORMAT_OPER_FSET, DEFAULT_FMT_OPER_FSET);

	set_fset_var (FORMAT_IGNORE_INVITE_FSET, DEFAULT_FMT_IGNORE_INVITE_FSET);
	set_fset_var (FORMAT_IGNORE_MSG_FSET, DEFAULT_FMT_IGNORE_MSG_FSET);
	set_fset_var (FORMAT_IGNORE_MSG_AWAY_FSET, DEFAULT_FMT_IGNORE_MSG_AWAY_FSET);
	set_fset_var (FORMAT_IGNORE_NOTICE_FSET, DEFAULT_FMT_IGNORE_NOTICE_FSET);
	set_fset_var (FORMAT_IGNORE_WALL_FSET, DEFAULT_FMT_IGNORE_WALL_FSET);
	set_fset_var (FORMAT_MSG_GROUP_FSET, DEFAULT_FMT_MSG_GROUP_FSET);

	set_fset_var (FORMAT_NAMES_FSET, DEFAULT_FMT_NAMES_FSET);
	set_fset_var (FORMAT_NAMES_NICKCOLOR_FSET, DEFAULT_FMT_NAMES_NICKCOLOR_FSET);
	set_fset_var (FORMAT_NAMES_NONOP_FSET, DEFAULT_FMT_NAMES_NONOP_FSET);
	set_fset_var (FORMAT_NAMES_VOICECOLOR_FSET, DEFAULT_FMT_NAMES_VOICECOLOR_FSET);
	set_fset_var (FORMAT_NAMES_OP_FSET, DEFAULT_FMT_NAMES_OP_FSET);
	set_fset_var (FORMAT_NAMES_IRCOP_FSET, DEFAULT_FMT_NAMES_IRCOP_FSET);
	set_fset_var (FORMAT_NAMES_VOICE_FSET, DEFAULT_FMT_NAMES_VOICE_FSET);
	set_fset_var (FORMAT_NAMES_OPCOLOR_FSET, DEFAULT_FMT_NAMES_OPCOLOR_FSET);

	set_fset_var (FORMAT_NETADD_FSET, DEFAULT_FMT_NETADD_FSET);
	set_fset_var (FORMAT_NETJOIN_FSET, DEFAULT_FMT_NETJOIN_FSET);
	set_fset_var (FORMAT_NETSPLIT_FSET, DEFAULT_FMT_NETSPLIT_FSET);
	set_fset_var (FORMAT_NICKNAME_FSET, DEFAULT_FMT_NICKNAME_FSET);
	set_fset_var (FORMAT_NICKNAME_OTHER_FSET, DEFAULT_FMT_NICKNAME_OTHER_FSET);
	set_fset_var (FORMAT_NICKNAME_USER_FSET, DEFAULT_FMT_NICKNAME_USER_FSET);
	set_fset_var (FORMAT_NONICK_FSET, DEFAULT_FMT_NONICK_FSET);


	set_fset_var (FORMAT_NOTE_FSET, DEFAULT_FMT_NOTE_FSET);


	set_fset_var (FORMAT_NOTICE_FSET, DEFAULT_FMT_NOTICE_FSET);

	set_fset_var (FORMAT_NOTIFY_SIGNOFF_FSET, DEFAULT_FMT_NOTIFY_SIGNOFF_FSET);
	set_fset_var (FORMAT_NOTIFY_SIGNOFF_UH_FSET, DEFAULT_FMT_NOTIFY_SIGNOFF_UH_FSET);
	set_fset_var (FORMAT_NOTIFY_SIGNON_UH_FSET, DEFAULT_FMT_NOTIFY_SIGNON_UH_FSET);
	set_fset_var (FORMAT_NOTIFY_SIGNON_FSET, DEFAULT_FMT_NOTIFY_SIGNON_FSET);
	set_fset_var (FORMAT_PUBLIC_FSET, DEFAULT_FMT_PUBLIC_FSET);
	set_fset_var (FORMAT_PUBLIC_AR_FSET, DEFAULT_FMT_PUBLIC_AR_FSET);
	set_fset_var (FORMAT_PUBLIC_MSG_FSET, DEFAULT_FMT_PUBLIC_MSG_FSET);
	set_fset_var (FORMAT_PUBLIC_MSG_AR_FSET, DEFAULT_FMT_PUBLIC_MSG_AR_FSET);
	set_fset_var (FORMAT_PUBLIC_NOTICE_FSET, DEFAULT_FMT_PUBLIC_NOTICE_FSET);
	set_fset_var (FORMAT_PUBLIC_NOTICE_AR_FSET, DEFAULT_FMT_PUBLIC_NOTICE_AR_FSET);
	set_fset_var (FORMAT_PUBLIC_OTHER_FSET, DEFAULT_FMT_PUBLIC_OTHER_FSET);
	set_fset_var (FORMAT_PUBLIC_OTHER_AR_FSET, DEFAULT_FMT_PUBLIC_OTHER_AR_FSET);
	set_fset_var (FORMAT_SEND_ACTION_FSET, DEFAULT_FMT_SEND_ACTION_FSET);
	set_fset_var (FORMAT_SEND_ACTION_OTHER_FSET, DEFAULT_FMT_SEND_ACTION_OTHER_FSET);
	set_fset_var (FORMAT_SEND_AWAY_FSET, DEFAULT_FMT_SEND_AWAY_FSET);
	set_fset_var (FORMAT_SEND_CTCP_FSET, DEFAULT_FMT_SEND_CTCP_FSET);
	set_fset_var (FORMAT_SEND_DCC_CHAT_FSET, DEFAULT_FMT_SEND_DCC_CHAT_FSET);
	set_fset_var (FORMAT_SEND_MSG_FSET, DEFAULT_FMT_SEND_MSG_FSET);
	set_fset_var (FORMAT_SEND_NOTICE_FSET, DEFAULT_FMT_SEND_NOTICE_FSET);
	set_fset_var (FORMAT_SEND_PUBLIC_FSET, DEFAULT_FMT_SEND_PUBLIC_FSET);
	set_fset_var (FORMAT_SEND_PUBLIC_OTHER_FSET, DEFAULT_FMT_SEND_PUBLIC_OTHER_FSET);
	set_fset_var (FORMAT_SERVER_FSET, DEFAULT_FMT_SERVER_FSET);
	set_fset_var (FORMAT_SERVER_MSG1_FSET, DEFAULT_FMT_SERVER_MSG1_FSET);
	set_fset_var (FORMAT_SERVER_MSG1_FROM_FSET, DEFAULT_FMT_SERVER_MSG1_FROM_FSET);
	set_fset_var (FORMAT_SERVER_MSG2_FSET, DEFAULT_FMT_SERVER_MSG2_FSET);
	set_fset_var (FORMAT_SERVER_MSG2_FROM_FSET, DEFAULT_FMT_SERVER_MSG2_FROM_FSET);

	set_fset_var (FORMAT_SERVER_NOTICE_FSET, DEFAULT_FMT_SERVER_NOTICE_FSET);

	set_fset_var (FORMAT_SET_FSET, DEFAULT_FMT_SET_FSET);
	set_fset_var (FORMAT_SET_NOVALUE_FSET, DEFAULT_FMT_SET_NOVALUE_FSET);
	set_fset_var (FORMAT_SIGNOFF_FSET, DEFAULT_FMT_SIGNOFF_FSET);


	set_fset_var (FORMAT_SILENCE_FSET, DEFAULT_FMT_SILENCE_FSET);

	set_fset_var (FORMAT_TRACE_OPER_FSET, DEFAULT_FMT_TRACE_OPER_FSET);
	set_fset_var (FORMAT_TRACE_SERVER_FSET, DEFAULT_FMT_TRACE_SERVER_FSET);
	set_fset_var (FORMAT_TRACE_USER_FSET, DEFAULT_FMT_TRACE_USER_FSET);

	set_fset_var (FORMAT_TIMER_FSET, DEFAULT_FMT_TIMER_FSET);
	set_fset_var (FORMAT_TOPIC_FSET, DEFAULT_FMT_TOPIC_FSET);
	set_fset_var (FORMAT_TOPIC_CHANGE_FSET, DEFAULT_FMT_TOPIC_CHANGE_FSET);
	set_fset_var (FORMAT_TOPIC_SETBY_FSET, DEFAULT_FMT_TOPIC_SETBY_FSET);
	set_fset_var (FORMAT_TOPIC_UNSET_FSET, DEFAULT_FMT_TOPIC_UNSET_FSET);

	set_fset_var (FORMAT_USAGE_FSET, DEFAULT_FMT_USAGE_FSET);
	set_fset_var (FORMAT_USERMODE_FSET, DEFAULT_FMT_USERMODE_FSET);

	set_fset_var (FORMAT_USERS_FSET, DEFAULT_FMT_USERS_FSET);
	set_fset_var (FORMAT_USERS_USER_FSET, DEFAULT_FMT_USERS_USER_FSET);
	set_fset_var (FORMAT_USERS_HEADER_FSET, DEFAULT_FMT_USERS_HEADER_FSET);
	set_fset_var (FORMAT_VERSION_FSET, DEFAULT_FMT_VERSION_FSET);


	set_fset_var (FORMAT_WALL_FSET, DEFAULT_FMT_WALL_FSET);
	set_fset_var (FORMAT_WALL_AR_FSET, DEFAULT_FMT_WALL_AR_FSET);


	set_fset_var (FORMAT_WALLOP_FSET, DEFAULT_FMT_WALLOP_FSET);
	set_fset_var (FORMAT_WHO_FSET, DEFAULT_FMT_WHO_FSET);
	set_fset_var (FORMAT_WHOIS_AWAY_FSET, DEFAULT_FMT_WHOIS_AWAY_FSET);
	set_fset_var (FORMAT_WHOIS_CHANNELS_FSET, DEFAULT_FMT_WHOIS_CHANNELS_FSET);
	set_fset_var (FORMAT_WHOIS_HEADER_FSET, DEFAULT_FMT_WHOIS_HEADER_FSET);
	set_fset_var (FORMAT_WHOIS_IDLE_FSET, DEFAULT_FMT_WHOIS_IDLE_FSET);
	set_fset_var (FORMAT_WHOIS_SIGNON_FSET, DEFAULT_FMT_WHOIS_SIGNON_FSET);
	set_fset_var (FORMAT_WHOIS_NAME_FSET, DEFAULT_FMT_WHOIS_NAME_FSET);
	set_fset_var (FORMAT_WHOIS_NICK_FSET, DEFAULT_FMT_WHOIS_NICK_FSET);
	set_fset_var (FORMAT_WHOIS_OPER_FSET, DEFAULT_FMT_WHOIS_OPER_FSET);
	set_fset_var (FORMAT_WHOIS_SERVER_FSET, DEFAULT_FMT_WHOIS_SERVER_FSET);
	set_fset_var (FORMAT_WHOLEFT_HEADER_FSET, DEFAULT_FMT_WHOLEFT_HEADER_FSET);
	set_fset_var (FORMAT_WHOLEFT_USER_FSET, DEFAULT_FMT_WHOLEFT_USER_FSET);
	set_fset_var (FORMAT_WHOWAS_HEADER_FSET, DEFAULT_FMT_WHOWAS_HEADER_FSET);
	set_fset_var (FORMAT_WHOWAS_NICK_FSET, DEFAULT_FMT_WHOWAS_NICK_FSET);
	set_fset_var (FORMAT_WIDELIST_FSET, DEFAULT_FMT_WIDELIST_FSET);
	set_fset_var (FORMAT_WINDOW_SET_FSET, DEFAULT_FMT_WINDOW_SET_FSET);

	set_fset_var (FORMAT_NICK_MSG_FSET, DEFAULT_FMT_NICK_MSG_FSET);

	set_fset_var (FORMAT_NICK_COMP_FSET, DEFAULT_FMT_NICK_COMP_FSET);
	set_fset_var (FORMAT_NICK_AUTO_FSET, DEFAULT_FMT_NICK_AUTO_FSET);

	set_fset_var (FORMAT_STATUS_FSET, DEFAULT_FMT_STATUS_FSET);
	set_fset_var (FORMAT_STATUS2_FSET, DEFAULT_FMT_STATUS2_FSET);
	set_fset_var (FORMAT_NOTIFY_OFF_FSET, DEFAULT_FMT_NOTIFY_OFF_FSET);
	set_fset_var (FORMAT_NOTIFY_ON_FSET, DEFAULT_FMT_NOTIFY_ON_FSET);

}

int 
save_formats (FILE * outfile)
{
	char thefile[BIG_BUFFER_SIZE + 1];
	char *p;
	int i;
	int count = 1;

	sprintf (thefile, "%s.formats", version);
	p = expand_twiddle (thefile);
	outfile = fopen (p, "w");
	if (!outfile)
	{
		bitchsay ("Cannot open file %s for saving!", thefile);
		new_free (&p);
		return 1;
	}
	for (i = 0; i < NUMBER_OF_FSET; i++)
	{

		if (fset_array[i].string)
			fprintf (outfile, "SET %s %s\n", fset_array[i].name, fset_array[i].string);
		else
			fprintf (outfile, "SET -%s\n", fset_array[i].name);
		count++;
	}
	fclose (outfile);
	bitchsay ("Saved %d formats to %s", count, thefile);
	new_free (&p);
	return 0;
}

char *
make_fstring_var (char *var_name)
{
	int cnt, msv_index;

	upper (var_name);
	if ((find_fixed_array_item (fset_array, sizeof (FsetArray), NUMBER_OF_FSET, var_name, &cnt, &msv_index) == NULL))
		return NULL;
	if (cnt >= 0)
		return NULL;
	return m_strdup (fset_array[msv_index].string);
}
