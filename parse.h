/*
 * parse.h
 *
 * written by matthew green
 * copyright (c) 1993
 *
 * @(#)$Id$
 */

#ifndef __parse_h_
# define __parse_h_

char *PasteArgs(char **, int);
int BreakArgs(char *, char **, char **, int);
void parse_server(char *);
void irc2_parse_server(char *);
int annoy_kicks(int, char *, char *, char *, struct nick_list *);
int ctcp_flood_check(char *, char *, char *);
void load_scripts(void);
BanList *ban_is_on_channel(char *ban, struct channel *);

extern char *FromUserHost;
extern int doing_privmsg;
extern char *last_split_server;

#define WAIT_WHO 0
#define WAIT_BANS 1
#define WAIT_MODE 2

#define MAXPARA 15

#endif				/* __parse_h_ */
