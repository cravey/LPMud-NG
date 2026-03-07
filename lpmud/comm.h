#define MAX_TEXT	2048

struct interactive {
    int socket;
    struct object *ob;		/* Points to the associated object */
    struct sentence *input_to;	/* To be called with next input line ! */
    struct sockaddr_in addr;
    char *prompt;
    int closing;		/* True when closing this socket. */
    int do_close;		/* This is to be closed down. */
    char text[MAX_TEXT];
    int text_end;		/* first free char in buffer */
    int text_start;		/* where we are up to in player cmd buffer */
    struct interactive *snoop_on, *snoop_by;
    int from_portal;		/* True if this player has arrived through a portal */
    int portal_socket;		/* All messages should go through this socket, if open */
    int out_portal;		/* True if going out through a portal. */
    int noecho;			/* Don't echo lines */
    int last_time;		/* Time of last command executed */
    char *default_err_message;	/* This or What ? is printed when error */
};

#define L_WAITING_FOR_NAME	1
#define L_LOGGED_ON		2

#define BACKEND_IN_BACKGROUND

#define HOST_NAME	"milou.cd.chalmers.se"

#define MAX_SOCKET_PACKET_SIZE	1024	/* Wild guess. */

#define ULTRASNOOP 0
