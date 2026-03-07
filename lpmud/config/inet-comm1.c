#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define	TELOPTS
#include <arpa/telnet.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#ifndef sun
#include <fcntl.h>
#endif
#ifdef aix
#include <sys/select.h>
#endif
#include "lint.h"
#include "interpret.h"
#include "comm.h"
#include "object.h"
#include "config.h"
#include "sent.h"
/* #define INET_NTOA_OK This is defined (if wanted) in config.h */

int socket PROT((int, int, int));
int setsockopt PROT((int, int, int, char *, int));
int bind PROT((int, struct sockaddr *, int));
int listen PROT((int, int));
int accept PROT((int, struct sockaddr *, int *));
int select PROT((int, fd_set *, fd_set *, fd_set *, struct timeval *));
int getpeername PROT((int, struct sockaddr *, int *));

void bzero PROT((char *, int));
void telnet_neg PROT((char *, char *));
void shutdown(), set_prompt PROT((char *));

extern char *xalloc(), *string_copy(), *unshared_str_copy();
extern int d_flag;
extern int current_time;
char * first_cmd_in_buf PROT((struct interactive *));
void next_cmd_in_buf PROT((struct interactive *));
char * skip_eols PROT((struct interactive *, char *));

extern void remove_interactive(), add_ref();

extern struct value *clone_object();

extern void logon(), debug_message(), fatal(), free_sentence();

struct interactive *all_players[MAX_PLAYERS];

extern int errno;

void new_player();

fd_set readfds;
int nfds = 0;
int num_player;

/*
 * Interprocess communication interface to the backend.
 */

static s;
extern int port_number;

void prepare_ipc() {
    struct sockaddr_in sin;
    struct hostent *hp;
    int tmp;
    char host_name[100];

    if (gethostname(host_name, sizeof host_name) == -1) {
        perror("gethostname");
	fatal("Error in gethostname()\n");
    }
    hp = gethostbyname(host_name);
    if (hp == 0) {
	(void)fprintf(stderr, "gethostbyname: unknown host.\n");
	exit(1);
    }
    memset((char *)&sin, '\0', sizeof sin);
    memcpy((char *)&sin.sin_addr, hp->h_addr, hp->h_length);
    sin.sin_port = htons(port_number);
    sin.sin_family = hp->h_addrtype;
    sin.sin_addr.s_addr = INADDR_ANY;
    s = socket(hp->h_addrtype, SOCK_STREAM, 0);
    if (s == -1) {
	perror("socket");
	abort();
    }
    tmp = 1;
    if (setsockopt (s, SOL_SOCKET, SO_REUSEADDR,
		    (char *) &tmp, sizeof (tmp)) < 0) {
	perror ("setsockopt");
	exit (1);
    }
    if (bind(s, (struct sockaddr *)&sin, sizeof sin) == -1) {
	if (errno == EADDRINUSE) {
	    debug_message("Socket already bound!\n");
	    exit(errno);
	} else {
	    perror("bind");
	    abort();
	}
    }
    if (listen(s, 5) == -1) {
	perror("listen");
	abort();
    }
    tmp = 1;
#ifdef sun
    if (ioctl(s, FIONBIO, &tmp) == -1) {
	perror("ioctl socket FIONBIO");
	abort();
    }
#else /* sun */
    if (fcntl(s, F_SETFL, FNDELAY) == -1) {
	perror("ioctl socket FIONBIO");
	abort();
    }
#endif /* sun */
    signal(SIGPIPE, SIG_IGN);
}

/*
 * This one is called when shutting down the MUD.
 */
void ipc_remove() {
    (void)printf("Shutting down ipc...\n");
    close(s);
}

/*
 * Do not call remove_interactive() when write fails, as this may cause
 * command_giver to point to a deallocated object, which some callers
 * of add_message() can't handle.
 */
/*VARARGS1*/
void add_message(fmt, a1, a2, a3, a4, a5, a6, a7, a8, a9)
    char *fmt;
    int a1, a2, a3, a4, a5, a6, a7, a8, a9;
{
    char buff[10000];		/* Kludgy! Hope this is enough ! */
    char buff2[sizeof buff];
    struct interactive *ip;
    int n, offset, chunk, length;
    int from, to;

    if (command_giver == 0 || command_giver->destructed)
	return;
    ip = command_giver->interactive;
    if (ip == 0 || ip->do_close)
	return;
    if (ip->out_portal) {
	buff[0] = ']';
	(void)sprintf(buff + 1, fmt, a1, a2, a3, a4, a5, a6, a7, a8, a9);
    } else {
	(void)sprintf(buff, fmt, a1, a2, a3, a4, a5, a6, a7, a8, a9);
    }
    length = strlen(buff);
    /*
     * Always check that your arrays are big enough ! :-)
     */
    if (length > sizeof buff)
	fatal("To long message!\n");
    if (ip->snoop_by) {
	struct object *save = command_giver;
	command_giver = ip->snoop_by->ob;
	add_message("%%%s", buff);
	command_giver = save;
    }
    if (d_flag)
	debug_message("[%s(%d)]: %s", command_giver->name, length, buff);
    /*
     * Insert CR after all NL.
     */
    for (from = 0, to = 0; to < sizeof buff2 && buff[from] != '\0';) {
	if (buff[from] == '\n')
	    buff2[to++] = '\r';
	buff2[to++] = buff[from++];
    }
    buff2[to++] = '\0';
    length = to-1;
    /*
     * We split up the message into something smaller than the max size.
     */
    for (offset=0; length > 0; offset += chunk, length -= chunk) {
	chunk = length;
	if (chunk > MAX_SOCKET_PACKET_SIZE)
	    chunk = MAX_SOCKET_PACKET_SIZE;
	if ((n = write(ip->socket, buff2 + offset, chunk)) == -1) {
	    if (errno == EMSGSIZE) {
		fprintf(stderr, "comm1: write EMSGSIZE.\n");
		return;
	    }
	    if (errno == EINVAL) {
		fprintf(stderr, "comm1: write EINVAL.\n");
		ip->do_close = 1;
		return;
	    }
	    if (errno == ENETUNREACH) {
		fprintf(stderr, "comm1: write ENETUNREACH.\n");
		ip->do_close = 1;
		return;
	    }
	    if (errno == EHOSTUNREACH) {
		fprintf(stderr, "comm1: write EHOSTUNREACH.\n");
		ip->do_close = 1;
		return;
	    }
	    if (errno == EPIPE) {
		fprintf(stderr, "comm1: write EPIPE detected\n");
		ip->do_close = 1;
		return;
	    }
	    if (errno == EWOULDBLOCK) {
		fprintf(stderr, "comm1: write EWOULDBLOCK. Message discarded.\n");
		ip->do_close = 1;
		return;
	    }
	    fprintf(stderr, "write: unknown errno %d\n", errno);
	    perror("write");
		ip->do_close = 1;
	    return;
	}
	if (n != chunk)
	    fprintf(stderr, "write socket: Size %d(%d) is to big !\n",
		    chunk, n);
	continue;
    }
}

/*
 * Copy a string, replacing newlines with '\0'. Also add an extra
 * space and back space for every newline. This trick will allow
 * otherwise empty lines, as multiple newlines would be replaced by
 * multiple zeroes only.
 */
static int copy_chars(from, to, n)
    char *from, *to;
    int n;
{
    int i;
    char *start = to;
    for (i=0; i<n; i++) {
	if (from[i] == '\r')
	    continue;
	if (from[i] == '\n') {
	    *to++ = ' ';
	    *to++ = '\b';
	    *to++ = '\0';
	    continue;
	}
	*to++ = from[i];
    }
    return to - start;
}

/*
 * Get a message from any player.  For all players without a completed
 * cmd in their input buffer, read their socket.  Then, return the first
 * cmd of the next player in sequence that has a complete cmd in their buffer.
 * CmdsGiven is used to allow people in ED to send more cmds (if they have
 * them queued up) than normal players.  If we get a heartbeat, still read
 * all sockets; if the next cmd giver is -1, we have already cycled and
 * can go back to do the heart beat.
 */

#define	StartCmdGiver	(MAX_PLAYERS-1) /* the one after heartbeat */
#define IncCmdGiver	NextCmdGiver = (NextCmdGiver < 0? StartCmdGiver: \
					NextCmdGiver - 1)

int NextCmdGiver = StartCmdGiver;
int CmdsGiven = 0;	 /* -1 is used to poll heart beat. */
int twait = 0;		 /* wait time for select() */
extern int time_to_call_heart_beat, comm_time_to_call_heart_beat;

int get_message(buff, size)
    char *buff;
    int size;
{
    int i, res;
    struct interactive * ip = 0;
    char *p, *p2;

    /*
     * Stay in this loop until we have a message from a player.
     */
    while(1) {
	int new_socket;
	struct sockaddr_in addr;
	int length;
	struct timeval timeout;

	/* First, try to get a new player... */
	length = sizeof addr;
	new_socket = accept(s, (struct sockaddr *)&addr, &length);
	if (new_socket != -1)
	    new_player(new_socket, &addr, length);
	else if (new_socket == -1 && errno != EWOULDBLOCK && errno != EINTR) {
	    perror("accept");
	    abort();
	}

	nfds = 0;
	FD_ZERO(&readfds);
	for (i=0; i<MAX_PLAYERS; i++) {
	    ip = all_players[i];
	    if (!ip)
		continue;
	    if (ip->do_close) {
		ip->do_close = 0;
		remove_interactive(ip->ob);
		continue;
	    }
	    if (!first_cmd_in_buf(ip)) {
		FD_SET(ip->socket, &readfds);
		if (ip->socket >= nfds)
		    nfds = ip->socket+1;
		if (ip->out_portal) {
		    FD_SET(ip->portal_socket, &readfds);
		    if (ip->portal_socket >= nfds)
			nfds = ip->portal_socket + 1;
		}
	    }
	}
	timeout.tv_sec = twait; /* avoid busy waiting when no buffered cmds */
	timeout.tv_usec = 0;
	res = select(nfds, &readfds, 0, 0, &timeout);
	if (res == -1) {
	    twait = 0;
	    if (errno == EINTR) /* if we got an alarm, finish the round */
		goto return_next_command;
	    perror("select");
	    return 0;
	}
	if (res) { /* waiting packets */
	    for (i=0; i<MAX_PLAYERS; i++) { /* read all pending sockets */
		struct interactive *ip = all_players[i];
		int x;
		if (ip == 0)
		    continue;
		if (FD_ISSET(ip->socket, &readfds)) { /* read this player */
		    char *p;
		    int l;

		    /* dont overfill their buffer */
		    l = MAX_TEXT - ip->text_end - 1;
		    if (l < size)
			size = l;

		    if ((l = read(ip->socket, buff, size)) == -1) {
			if (errno == ENETUNREACH) {
			    debug_message("Net unreachable detected.\n");
			    remove_interactive(ip->ob);
			    continue;
			}
			if (errno == EHOSTUNREACH) {
			    debug_message("Host unreachable detected.\n");
			    remove_interactive(ip->ob);
			    continue;
			}
			if (errno == ETIMEDOUT) {
			    debug_message("Connection timed out detected.\n");
			    remove_interactive(ip->ob);
			    continue;
			}
			if (errno == ECONNRESET) {
			    debug_message("Connection reset by peer detected.\n");
			    remove_interactive(ip->ob);
			    continue;
			}
			if (errno == EWOULDBLOCK) {
			    debug_message("read would block socket %d!\n",
					  ip->socket);
			    remove_interactive(ip->ob);
			    continue;
			}
			if (errno == EMSGSIZE) {
			    debug_message("read EMSGSIZE !\n");
			    continue;
			}
			perror("read");
			debug_message("Unknown errno %d\n", errno);
			remove_interactive(ip->ob);
			continue;
		    }
		    /*
		     * IF the data goes through a portal, send it,
		     * but don't return any data.
		     */
		    if (ip->out_portal) {
			if (ip->text_end) { /* pending text first */
			    write(ip->portal_socket, ip->text, ip->text_end);
			    ip->text_end = 0;
			}
			if (l)
			    write(ip->portal_socket, buff, l);
			continue;
		    }
		    if (l == 0) {
			if (ip->closing)
			    fatal("Tried to read from closing socket.\n");
			remove_interactive(ip->ob);
			return 0;
		    }
		    buff[l] = '\0';
		    /* replace newlines by nulls and catenate to buffer */
		    ip->text_end +=
			copy_chars(buff, ip->text + ip->text_end, l);
#if 0
		    for (x=0; x<l; x++) {
			if (buff[x] == '\n' || buff[x] == '\r')
			    buff[x] = '\0';
		    }
		    memcpy(ip->text+ip->text_end, buff, l+1);
		    ip->text_end += l;
#endif
		    /* now, text->end is just after the last char read.  If last */
		    /* char was a nl, char *before* text_end will be null. */
		    ip->text[ip->text_end] = '\0';
		}
	    }
	}
	/*
	 * we have read the sockets; now find and return a command
	 */
    return_next_command:
	twait = 0;
	ip = 0;
	for (i = 0; i < MAX_PLAYERS + 1; i++) {
	    if (NextCmdGiver == -1) { /* we have cycled around all players */
		CmdsGiven = 0;	  /* check heart beat */
		IncCmdGiver;
		if (comm_time_to_call_heart_beat) {
		    time_to_call_heart_beat = 1; /* twait stays 0! */
		    return 0;
		}
	    }
	    ip = all_players[NextCmdGiver];
	    if (ip && (p = first_cmd_in_buf(ip))) /* wont respond to partials */
		break;
	    CmdsGiven = 0; /* new player, no cmds issued */
	    IncCmdGiver;
	}
	
	if ((!ip)||!p) { /* no cmds found; loop and select (on timeout) again */
	    if (comm_time_to_call_heart_beat) { /* may as well do it now */
		time_to_call_heart_beat = 1; /* no cmds, do heart beat */
		NextCmdGiver = StartCmdGiver;/* do a complete poll next time */
		CmdsGiven = 0;
		return(0);
	    }
	    /* no heart beat to do and no cmds pending - avoid busy wait on select */
	    twait = 1;
	    continue; /* else await another cmd */
	}
	
	/*
	 * we have a player cmd - return it.  If he is in ed, count his
	 * cmds, else only allow 1 cmd.  If he has only one partially
	 * completed cmd left after * this, move it to the start of his
	 * buffer; new stuff will be appended.
	 */
	
	command_giver = ip->ob;
	telnet_neg(buff, p);
	next_cmd_in_buf(ip); /* move on buffer pointers */
	
	/* if he is not in ed, dont let him issue another till the poll comes again */
	
	if (command_giver->ed_buffer && (CmdsGiven < ALLOWED_ED_CMDS))
	    CmdsGiven++;
	else {
	    IncCmdGiver;
	    CmdsGiven = 0;
	}
	
	/* manage snooping - should the snooper see type ahead?  Well, he doesn't here
	 */
	if (ip->snoop_by && !ip->noecho) {
	    command_giver = ip->snoop_by->ob;
	    add_message("%% %s\n", buff);
	}
	command_giver = ip->ob;
	if (ip->noecho) {
	    /* Must not enable echo before the user input is received. */
	    add_message("%c%c%c", IAC, WONT, TELOPT_ECHO);
	}
	ip->noecho = 0;
	ip->last_time = current_time;
	return 1;
    }
}

/*
 * find the first character of the next complete cmd in a buffer, 0 if no
 * completed cmd.  There is a completed cmd if there is a null between
 * text_start and text_end.  Zero length commands are discarded (as occur
 * between <cr> and <lf>).  Update text_start if we have to skip leading
 * nulls.
 */

char * first_cmd_in_buf(ip) 
struct interactive *ip;
{
    char * p, *q;

    p = ip->text_start + ip->text;

    while ((p < (ip->text_end + ip->text)) && !*p) /* skip null input */
	p++;

    ip->text_start = p - ip->text;

    if (ip->text_start >= ip->text_end) {
	ip->text_start = ip->text_end = 0;
	ip->text[0] = '\0';
	return(0);
    }

    while ((p < (ip->text_end + ip->text)) && *p) /* find end of cmd */
	p++;

    if (p < ip->text + ip->text_end) /* null terminated, was command */
	return(ip->text + ip->text_start);

/* have a partial command at end of buffer; move it to start, return null */
/* if it can't move down, truncate it and return it as cmd. */
    
    p = ip->text + ip->text_start;
    q = ip->text;
    while (p < (ip->text + ip->text_end))
	*(q++) = *(p++);

    ip->text_end -= ip->text_start;
    ip->text_start = 0;
    if (ip->text_end > MAX_TEXT - 2) {
	ip->text[ip->text_end-2] = '\0'; /* nulls to truncate */
	ip->text[ip->text_end-1] = '\0'; /* nulls to truncate */
	ip->text_end--;
	return(ip->text);
	}
/* buffer not full and no newline - no cmd. */
    return(0);
}

/*
 * move pointers to next cmd, or clear buf.
 */

void next_cmd_in_buf(ip)
struct interactive *ip;
{
    char * p = ip->text + ip->text_start;
    while (*p && p < ip->text + ip->text_end)
	p++;
    /* skip past any nulls at the end */
    while (!*p && p < ip->text + ip->text_end)
	p++;
    if (p < ip->text + ip->text_end)
	ip->text_start = p - ip->text;
    else {
	ip->text_start = ip->text_end = 0;
	ip->text[0] = '\0';
    }
}
    

/*
 * Remove an interactive player immediately.
 */
void remove_interactive(ob)
    struct object *ob;
{
    struct object *save = command_giver;
    int i;

    for (i=0; i<MAX_PLAYERS; i++) {
	if (all_players[i] != ob->interactive)
	    continue;
	if (ob->interactive->closing)
	    fatal("Double call to remove_interactive()\n");
	ob->interactive->closing = 1;
	if (ob->interactive->snoop_by) {
	    ob->interactive->snoop_by->snoop_on = 0;
	    ob->interactive->snoop_by = 0;
	}
	if (ob->interactive->snoop_on) {
	    ob->interactive->snoop_on->snoop_by = 0;
	    ob->interactive->snoop_on = 0;
	}
	command_giver = ob;
	add_message("Closing down.\n");
	(void)shutdown(ob->interactive->socket, 2);
	close(ob->interactive->socket);
	num_player--;
	if (ob->interactive->input_to) {
	    free_object(ob->interactive->input_to->ob, "remove_interactive");
	    free_sentence(ob->interactive->input_to);
	    ob->interactive->input_to = 0;
	}
	free((char *)ob->interactive);
	ob->interactive = 0;
	all_players[i] = 0;
	free_object(ob, "remove_interactive");
	command_giver = save;
	return;
    }
    (void)fprintf(stderr, "Could not find and remove player %s\n", ob->name);
    abort();
}

int
allow_host_access(new_socket)
    int new_socket;
{
    struct sockaddr_in apa;
    int len;
    char * ipname, *calloc(), *xalloc(), *index();
    static int read_access_list = 0;
    static struct access_list {
	int addr_len;
	char * addr, *name, *comment;
	struct access_list * next;
    } * access_list;
    register struct access_list * ap;

    if(!read_access_list) {
	FILE * f = fopen("ACCESS.DENY", "r");
	char buf[1024], ipn[50], hname[100], comment[1024], *p1, *p2;
	struct access_list * na;
	struct hostent * hent;

	read_access_list = 1;
	if(f) {
	    while(fgets(buf, sizeof buf - 1, f)) {
		if(*buf != '#') {
		    ipn[0] = hname[0] = comment[0] = 0;
		    if(p1 = index(buf, ':')) *p1 = 0;
		    if(buf[0] && buf[0] != '\n')
			strncpy(ipn, buf, sizeof ipn - 1);
		    if((p2 = p1) && *++p2) {
			if(p1 = index(p2, ':')) *p1 = 0;
			if(p2[0] && p2[0] != '\n')
			    strcpy(hname, p2);
			if(p1 && p1[1] && p1[1] != '\n')
			    strcpy(comment, p1+1);
		    }
			
		    if(!(na = (struct access_list *)xalloc(sizeof na[0]))) {
			fatal("Out of mem.\n");
		    }
		    na->addr = na->name = na->comment = 0;
		    na->next = 0;
		    if(*ipn && (!(na->addr = xalloc(strlen(ipn) + 1)) ||
				!strcpy(na->addr, ipn)))
			fatal("Out of mem.\n");
		    if(*hname && (!(na->name = xalloc(strlen(hname) + 1)) ||
				  !strcpy(na->name, hname)))
			fatal("Out of mem.\n");
		    if(*comment && (!(na->comment=xalloc(strlen(comment)+1))||
				    !strcpy(na->comment, comment)))
			fatal("Out of mem.\n");

		    if((!(int)*ipn)
			&&
			((!*hname)
			  || (!(hent = gethostbyname(hname))) ||
				 (!(na->addr =
				   xalloc(hent->h_length+1)))||
				 !strcpy(na->addr,
					inet_ntoa(*(long *)hent->h_addr)))) {
			if(na->name) free(na->name);
			if(na->comment) free(na->comment);
			free((char *)na);
			continue;
		    }
		    if(!(na->addr_len = strlen(na->addr)))
			continue;

		    /* printf("disabling: %s:%s:%s\n", na->addr,
			   na->name?na->name:"no name",
			   na->comment?na->comment:"no comment");  */

		    na->next = access_list;
		    access_list = na;
		}
	    }
	    fclose(f);
	}
    }
    
    if (!access_list)
	return 0;
	
    if(getpeername(new_socket, (struct sockaddr *)&apa, &len) == -1) {
	perror("getpeername");
	return -1;
    }

    ipname = inet_ntoa(apa.sin_addr);
    
    for(ap = access_list; ap; ap = ap->next)
	if(!strncmp(ipname, ap->addr, ap->addr_len)){
	    if(ap->comment) (void) write(new_socket, ap->comment,
					 strlen(ap->comment));
	    printf("Stopping: %s:%s\n", ap->addr, ap->name?ap->name:"no name");
	    close(new_socket);
	    return -1;
	}
    return 0;
}

/*
 * get the I'th player object from the interactive list, i starts at 0
 * and can go to num_player - 1.  For users(), etc.
 */
struct object * get_interactive_object(i)
int i;
{
    int n;

    if (i >= num_player) /* love them ASSERTS() :-) */
	fatal("Get interactive (%d) with only %d players!", i, num_player);

    for (n = 0; n < MAX_PLAYERS; n++)
	if (all_players[n])
	    if (!(i--))
		return(all_players[n]->ob);

    fatal("Get interactive: player %d not found! (num_players = %d)",
		i, num_player);
}

void new_player(new_socket, addr, len)
    int new_socket;
    struct sockaddr_in *addr;
    int len;
{
    int i;
    char *p;
    
    if(allow_host_access(new_socket))
	return;
    if (d_flag)
	debug_message("New player at socket %d.\n", new_socket);
    for (i=0; i<MAX_PLAYERS; i++) {
	struct object *ob;
	
	if (all_players[i] != 0)
	    continue;
	current_object = 0;
	ob = (clone_object("obj/player"))->u.ob;
	if (ob == 0)
	    fatal("Could not load 'obj/player'\n");
	add_ref(ob, "new_player");
	ob->interactive =
	    (struct interactive *)xalloc(sizeof (struct interactive));
	ob->interactive->default_err_message = 0;
	ob->once_interactive = 1;
	all_players[i] = ob->interactive;
	command_giver = ob;
	ob->interactive->ob = ob;
	ob->interactive->text[0] = '\0';
	ob->interactive->input_to = 0;
	ob->interactive->closing = 0;
	ob->interactive->snoop_on = 0;
	ob->interactive->snoop_by = 0;
	ob->interactive->out_portal = 0;
	ob->interactive->portal_socket = 0;
	ob->interactive->from_portal = 0;
	ob->interactive->text_end = 0;
	ob->interactive->text_start = 0;
	ob->interactive->do_close = 0;
	ob->interactive->noecho = 0;
	set_prompt("> ");
	all_players[i]->socket = new_socket;
	/* memcpy(&all_players[i]->addr, addr, len); */
	getpeername(new_socket, (struct sockaddr *)&all_players[i]->addr,
		    &len);
	num_player++;
	logon(ob);
	return;
    }
    p = "Lpmud is full. Come back later.\r\n";
    write(new_socket, p, strlen(p));
    close(new_socket);
}

int call_function_interactive(i, str)
    struct interactive *i;
    char *str;
{
    char *function;
    struct object *ob;
    struct value *val;

    if (!i->input_to)
	return 0;
    /*
     * Special feature: input_to() has been called to setup
     * a call to a function.
     */
    if (i->input_to->ob->destructed) {
	/* Sorry, the object has selfdestructed ! */
	free_object(i->input_to->ob, "call_function_interactive");
	free_sentence(i->input_to);
	i->input_to = 0;
	return 0;
    }
    free_object(i->input_to->ob, "call_function_interactive");
    function = string_copy(command_giver->interactive->input_to->function);
    ob = i->input_to->ob;
    val = make_string(str);
    free_sentence(i->input_to);
    /*
     * We must clear this reference before the call to apply(), because
     * someone might want to set up a new input_to().
     */
    i->input_to = 0;
    /*
     * Now we set current_object to this object, so that input_to will
     * work for static functions.
     */
    current_object = ob;
    (void)apply(function, ob, val);
    free(function);
    return 1;
}

int set_call(ob, sent, noecho)
    struct object *ob;
    struct sentence *sent;
    int noecho;
{
    struct object *save = command_giver;
    if (ob == 0 || sent == 0)
	return 0;
    if (ob->interactive == 0 || ob->interactive->input_to)
	return 0;
    ob->interactive->input_to = sent;
    ob->interactive->noecho = noecho;
    command_giver = ob;
    if (noecho)
	add_message("%c%c%c", IAC, WILL, TELOPT_ECHO);
    command_giver = save;
    return 1;
}

void show_info_about(str, room, i)
    char *str, *room;
    struct interactive *i;
{
    struct hostent *hp = 0;

#if 0
    hp = gethostbyaddr(&i->addr.sin_addr.s_addr, 4, AF_INET);
#endif
    add_message("%-15s %-15s %s\n",
		hp ? hp->h_name : inet_ntoa(i->addr.sin_addr), str, room);
}

void remove_all_players()
{
    int i;

    for (i=0; i<MAX_PLAYERS; i++) {
	if (all_players[i] == 0)
	    continue;
	command_giver = all_players[i]->ob;
	(void)apply("quit", all_players[i]->ob, 0);
    }
}

void set_prompt(str)
    char *str;
{
    command_giver->interactive->prompt = str;
}

/*
 * Print the prompt, but only if input_to not is enabled.
 */
void print_prompt()
{
    if (command_giver == 0)
	fatal("command_giver == 0.\n");
    if (command_giver->interactive->input_to == 0)
	add_message(command_giver->interactive->prompt);
}

void set_snoop(me, you)
    struct object *me, *you;
{
    struct interactive *on = 0, *by = 0;
    int i;
    
    for(i=0; i<MAX_PLAYERS && (on == 0 || by == 0); i++) {
	if (all_players[i] == 0)
	    continue;
	if (all_players[i]->ob == me)
	    by = all_players[i];
	else if (all_players[i]->ob == you)
	    on = all_players[i];
    }
    if (you == 0) {
	if (by == 0)
	    fatal("Could not find myself to stop snoop.\n");
	add_message("Ok.\n");
	if (by->snoop_on == 0)
	    return;
	by->snoop_on->snoop_by = 0;
	by->snoop_on = 0;
	return;
    }
    if (on == 0 || by == 0) {
	add_message("Failed.\n");
	return;
    }
    if (by->snoop_on)
	by->snoop_on->snoop_by = 0;
    if (on->snoop_by) {
	add_message("Busy.\n");
	return;
    }
    on->snoop_by = by;
    by->snoop_on = on;
    add_message("Ok.\n");
}

#define	TS_DATA		0
#define	TS_IAC		1
#define	TS_WILL		2
#define	TS_WONT		3
#define	TS_DO		4
#define	TS_DONT		5

void telnet_neg(to, from)
    char *to, *from;
{
    int state = TS_DATA;
    int ch;
    char *first = to;

    while(1) {
	ch = (*from++ & 0xff);
	switch(state) {
	case TS_DATA:
	    switch(ch) {
	    case IAC:
		state = TS_IAC;
		continue;
	    case '\b':	/* Backspace */
	    case 0x7f:	/* Delete */
		if (to <= first)
		    continue;
		to -= 1;
		continue;
	    default:
		if (ch & 0x80) {
		    debug_message("Tel_neg: 0x%x\n", ch);
		    continue;
		}
		*to++ = ch;
		if (ch == 0)
		    return;
		continue;
	    }
	case TS_IAC:
	    switch(ch) {
	    case WILL:
		state = TS_WILL;
		continue;
	    case WONT:
		state = TS_WONT;
		continue;
	    case DO:
		state = TS_DO;
		continue;
	    case DONT:
		state = TS_DONT;
		continue;
	    case DM:
		break;
	    case NOP:
	    case GA:
	    default:
		break;
	    }
	    state = TS_DATA;
	    continue;
	case TS_WILL:
	    debug_message("Will %s\n", telopts[ch]);
	    state = TS_DATA;
	    continue;
	case TS_WONT:
	    debug_message("Wont %s\n", telopts[ch]);
	    state = TS_DATA;
	    continue;
	case TS_DO:
	    debug_message("Do %s\n", telopts[ch]);
	    state = TS_DATA;
	    continue;
	case TS_DONT:
	    debug_message("Dont %s\n", telopts[ch]);
	    state = TS_DATA;
	    continue;
	default:
	    debug_message("Bad state: 0x%x\n", state);
	    state = TS_DATA;
	    continue;
	}
    }
}

#ifdef INET_NTOA_OK

char *query_ip_number(ob)
    struct object *ob;
{
    if (ob == 0)
	ob = command_giver;
    if (!ob || ob->interactive == 0)
	return 0;
    return inet_ntoa(ob->interactive->addr.sin_addr);
}

#else /* INET_NTOA_OK */
/*
Note: if the address string is "a.b.c.d" the address number is
      a * 256^3 + b * 256^2 + c * 256 + d

*/

char *query_ip_number(ob)
    struct object *ob;
{
    u_long s_ad;
    int a, b, c, d;
    static char addr[20]; /* 16 + 1 should be enough */

    if (ob == 0)
	ob = command_giver;
    if (!ob || ob->interactive == 0)
	return 0;
    s_ad = ob->interactive->addr.sin_addr.s_addr;
    d = s_ad % 256;
    s_ad /= 256;
    c = s_ad % 256;
    s_ad /= 256;
    b = s_ad % 256;
    a = s_ad / 256;
    sprintf(addr, "%d.%d.%d.%d", a, b, c, d);
    return addr;
}
#endif INET_NTOA_OK

char *query_host_name() {
    static char name[20];
    
    gethostname(name, sizeof name);
    name[sizeof name - 1] = '\0';	/* Just to make sure */
    return name;
}

struct value *query_snoop(ob)
    struct object *ob;
{
    struct value *arg1;
    if (ob->interactive->snoop_by == 0)
	return &const0;
    arg1 = alloc_value();
    arg1->type = T_OBJECT;
    arg1->u.ob = ob->interactive->snoop_by->ob;
    add_ref(arg1->u.ob, "query_snoop");
    return arg1;
}

int query_idle(ob)
    struct object *ob;
{
    if (!ob->interactive)
	error("query_idle() of non-interactive object.\n");
    return current_time - ob->interactive->last_time;
}

void notify_no_command() {
    char *p;

    if (!command_giver->interactive)
	return;
    p = command_giver->interactive->default_err_message;
    if (p) {
	add_message(p);
	free_string(p);
	command_giver->interactive->default_err_message = 0;
    } else {
	add_message("What ?\n");
    }
}

void clear_notify() {
    if (!command_giver->interactive)
	return;
    if (command_giver->interactive->default_err_message) {
	free_string(command_giver->interactive->default_err_message);
	command_giver->interactive->default_err_message = 0;
    }
}

void set_notify_fail_message(str)
    char *str;
{
    clear_notify();
    if (!command_giver || !command_giver->interactive)
	return;
    command_giver->interactive->default_err_message = make_shared_string(str);
}
