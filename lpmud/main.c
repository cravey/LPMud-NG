#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include "lint.h"
#include "lnode.h"
#include "interpret.h"
#include "object.h"
#include "config.h"

struct value const0, const1;

extern struct lnode *prog;

extern int current_line;

int d_flag = 0;	/* Run with debug */
int t_flag = 0;	/* Disable heart beat and reset */
int e_flag = 0;	/* Load empty, without castles. */
#ifdef YYDEBUG
extern int yydebug;
#endif

int port_number = PORTNO;
char *reserved_area;	/* reserved for malloc() */

double consts[5];

int main(int argc, char **argv)
{
    extern int current_time;
    int i;
    const char *mudlib;

    if (argc == 2) {
	if (strcmp(argv[1], "-e") == 0)
	    e_flag++;
	else if (strcmp(argv[1], "-d") == 0)
	    d_flag++;
	else if (strcmp(argv[1], "-t") == 0)
	    t_flag++;
#ifdef YYDEBUG
	else if (strcmp(argv[1], "-y") == 0)
	    yydebug = 1;
#endif
	else port_number = atoi(argv[1]);
    }
#ifdef DRAND48
    srand48(time(0));
#else
#ifdef RANDOM
    srandom(time(0));
#else
    fprintf(stderr, "No random generator specified!\n");
#endif /* RANDOM */
#endif /* DRAND48 */
    current_time = time(0);
    if (RESERVED_SIZE > 0)
	reserved_area = malloc(RESERVED_SIZE);
    for (i=0; (size_t)i < sizeof consts / sizeof consts[0]; i++)
	consts[i] = exp(- i / 900.0);
    init_string_space();
    mudlib = getenv("MUD_LIB");
    if (mudlib == NULL)
	mudlib = "./mudlib";
    if (chdir(mudlib) != 0) {
	perror(mudlib);
	exit(1);
    }
    init_constant_lnodes();
    compute_lnode_size();
    load_wiz_file();
    const0.type = T_NUMBER;
    const0.u.number = 0;
    const1.type = T_NUMBER;
    const1.u.number = 1;
    backend();
    return 0;
}

char *string_copy(char *str)
{
    char *p;

    p = xalloc(strlen(str)+1);
    (void)strcpy(p, str);
    return p;
}

void debug_message(char *fmt, ...)
{
    static FILE *fp = NULL;
    char deb[100];
    char name[100];
    va_list ap;

    if (fp == NULL) {
	gethostname(name,sizeof name);
	name[sizeof(name) - 1] = '\0';
	(void)snprintf(deb, sizeof deb, "%s.debug.log", name);
	fp = fopen(deb, "w");
	if (fp == NULL) {
	    perror(deb);
	    abort();
	}
    }
    va_start(ap, fmt);
    (void)vfprintf(fp, fmt, ap);
    va_end(ap);
    (void)fflush(fp);
}

void debug_message_value(struct value *v)
{
    if (v == 0) {
	debug_message("<NULL>");
	return;
    }
    switch(v->type) {
    case T_NUMBER:
	debug_message("%d", v->u.number);
	return;
    case T_STRING:
	debug_message("\"%s\"", v->u.string);
	return;
    case T_OBJECT:
	debug_message("OBJ(%s)", v->u.ob->name);
	return;
    default:
	fatal("<INVALID>\n");
	return;
    }
}

#ifdef malloc
#undef malloc
#endif

int slow_shut_down_to_do = 0;

static void log_xalloc_failure(size_t size, int err, int had_reserve)
{
    char buf[256];
    int n;

    n = snprintf(buf, sizeof(buf),
	"xalloc failed: size=%zu errno=%d (%s) reserve=%s\n",
	size, err, strerror(err), had_reserve ? "present" : "empty");
    if (n > 0)
	(void)write(2, buf, n);
}

char *xalloc(size_t size)
{
    char *p;
    int err;

    if (size == 0)
	fatal("Tried to allocate 0 bytes.\n");
    p = malloc(size);
    if (p == 0) {
	err = errno;
	if (reserved_area) {
	    log_xalloc_failure(size, err, 1);
	    free(reserved_area);
	    p = "Temporary out of MEMORY. Freeing reserve.\n";
	    write(1, p, strlen(p));
	    reserved_area = 0;
	    slow_shut_down_to_do = 6;
	    return xalloc(size);	/* Try again */
	}
	log_xalloc_failure(size, err, 0);
	p = "Totally out of MEMORY.\n";
	write(1, p, strlen(p));
	exit(2);
    }
    return p;
}
