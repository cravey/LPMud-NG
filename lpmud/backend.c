#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <time.h>
#include <math.h>
#include "lint.h"
#include "config.h"
#include "lnode.h"
#include "interpret.h"
#include "object.h"
#include "wiz_list.h"

jmp_buf error_recovery_context;
int error_recovery_context_exists = 0;
/*
 * The 'current_time' is updated at every heart beat with time(0).
 */
int current_time;

char *string_copy PROT((char *));
extern struct object *command_giver, *obj_list_destruct;
extern int num_player;

struct object *current_heart_beat;
struct object *current_reset;

void call_heart_beat(void), catch_alarm(int);
void load_first_objects(), prepare_ipc(),
    shutdowngame(), ed_cmd PROT((char *)),
    print_prompt(), move_current_reset(), call_out(),
    destruct2 PROT((struct object *));
void slow_shut_down PROT((int));
static void cycle_hb_list PROT((void));

extern int get_message PROT((char *, int)), player_parser PROT((char *)),
    call_function_interactive PROT((struct interactive *, char *)),
    resort_free_list(), swap PROT((struct object *));

extern int t_flag;
int time_to_call_heart_beat;
int comm_time_to_call_heart_beat = 0; /* this is set by interrupt, */
	/* comm sets time_to_call_heart_beat sometime after */

/*
 * There are global variables that must be zeroed before any execution.
 * In case of errors, there will be a longjmp(), and the variables will
 * have to be cleared explicitely. They are normally maintained by the
 * code that use them.
 */
void clear_state() {
    extern struct lnode_2 *next_arg_list_to_use;
    extern struct object *previous_ob, *current_inherit_ob;
    extern int static_variable_flag, break_flag, break_level;
#ifdef TRACE
    extern int trace_depth;
#endif

    break_flag = 0;
    break_level = 0;
    static_variable_flag = 0;
    next_arg_list_to_use = 0;
    current_object = 0;
    command_giver = 0;
    previous_ob = 0;
    current_inherit_ob = 0;
    error_recovery_context_exists = 1;
#ifdef TRACE
    trace_depth = 0;
#endif
}

void logon(struct object *ob)
{
    struct value *ret;

    /*
     * Set current object to ob, so that the static function "logon"
     * can be called.
     */
    current_object = ob;
    ret = apply("logon", ob, 0);
    if (ret == 0) {
	add_message("prog %s:\n", ob->name);
	fatal("Could not find logon on the player %s\n", ob->name);
    }
}

/*
 * Take a player command and parse it.
 * The command can also come from a NPC.
 * Beware that 'str' can be modified and extended !
 */
int parse_command(char *str, struct object *ob)
{
    struct object *save = command_giver;
    int res;

    command_giver = ob;
    res = player_parser(str);
    command_giver = save;
    return res;
}

/*
 * This is the backend. We will stay here for ever (almost).
 */
int eval_cost;
void backend()
{
    char buff[2000];
    extern void startshutdowngame(int);
    extern int game_is_being_shut_down;
    extern int slow_shut_down_to_do;

    free_all_values();
    (void)printf("Loading init file %s\n", INIT_FILE);
    load_first_objects();
    current_reset = obj_list;
    (void)printf("Setting up ipc.\n");
    fflush(stdout);
    prepare_ipc();
    (void)signal(SIGHUP, startshutdowngame);
    if (!t_flag)
	call_heart_beat();
    setjmp(error_recovery_context);
    /*
     * We come here after errors, and have to clear some global variables.
     */
    clear_state();
    while(1) {
	eval_cost = 0;
	remove_destructed_objects();
	if (game_is_being_shut_down)
	    shutdowngame();
	if (slow_shut_down_to_do) {
	    int tmp = slow_shut_down_to_do;
	    slow_shut_down_to_do = 0;
	    slow_shut_down(tmp);
	}
	if (get_message(buff, sizeof buff)) {
	    void update_load_av PROT((void));

	    update_load_av();
	    /*
	     * Now we have a string from the player. This string can go to
	     * one of several places. If it is prepended with a '!', then
	     * it is an escape from the 'ed' editor, so we send it
	     * as a command to the parser.
	     * If any object function is waiting for an input string, then
	     * send it there.
	     * Otherwise, send the string to the parser.
	     * The player_parser() will find that current_object is 0, and
	     * then set current_object to point to the object that defines
	     * the command. This will enable such functions to be static.
	     */
	    current_object = 0;
	    if (!command_giver->interactive)
		fatal("Non interactive player in main loop !\n");
	    if (command_giver->reset > 1)
		command_giver->reset = 1;
	    if (buff[0] == '!' && command_giver->super)
		parse_command(buff+1, command_giver);
	    else if (command_giver->ed_buffer)
		ed_cmd(buff);
	    else if (call_function_interactive(command_giver->interactive,buff))
		;	/* Do nothing ! */
	    else
		parse_command(buff, command_giver);
	    /*
	     * Print a prompt if player is still here.
	     */
	    if (command_giver->interactive)
		print_prompt();
	}
	command_giver = 0;
	if (time_to_call_heart_beat)
	    call_heart_beat();
	free_all_values();
    }
}

/*
 * Call all heart_beat() functions in all objects.  Also call the next reset,
 * and the call out.
 * We do heart beats by moving each object done to the end of the heart beat
 * list before we call its function, and always using the item at the head
 * of the list as our function to call.  We keep calling heart beats until
 * a timeout or we have done num_heart_objs calls.  It is done this way so
 * that objects can delete heart beating objects from the list from within
 * their heart beat without truncating the current round of heart beats.
 */
static struct object * hb_list = 0; /* head */
static struct object * hb_tail = 0; /* for sane wrap around */

static int num_hb_objs = 0;  /* so we know when to stop! */
static int num_hb_calls = 0; /* stats */
static float perc_hb_probes = 100.0; /* decaying avge of how many complete */

void call_heart_beat(void) {
    struct object *ob, *hide_current = current_object,
                  *hide_command = command_giver;
    int num_done = 0;

    time_to_call_heart_beat = 0; /* interrupt loop if we take too long */
    comm_time_to_call_heart_beat = 0;
    (void)signal(SIGALRM, catch_alarm);
    alarm(2);
    current_time = time(0l);

    if ((num_player > 0) && hb_list) {
        num_hb_calls++;
	while (hb_list && (!comm_time_to_call_heart_beat)
		  && (num_done < num_hb_objs)) {
	    num_done++;
	    cycle_hb_list();
	    ob = hb_tail; /* now at end */
	    if (ob->enable_heart_beat == 0)
		fatal("Heart beat not set in object on heart beat list!");
	    if (ob->swapped)
		fatal("Heart beat in swapped object.\n");
/* move ob to end of list, do ob */
	    if (ob->heart_beat == 0 || ob->reset == 0) continue;
	    current_object = ob;
	    current_heart_beat = ob;
	    if (ob->enable_commands)
		command_giver = ob;
	    else
		command_giver = 0;
	    if (ob->wl)
		ob->wl->heart_beats++;
	    eval_cost = 0;
	    call_function((struct lnode_def *)ob->heart_beat);
	    free_all_values();
	}
	if (num_hb_objs)
		perc_hb_probes = 100 * (float) num_done / num_hb_objs;
	else
		perc_hb_probes = 100.0;
        }
	command_giver = hide_command;
	current_object = hide_current;
	current_heart_beat = 0;
	move_current_reset();
	call_out();	/* some things depend on this, even without players! */
}

/*
 * Take the first object off the heart beat list, place it at the end
 */
static void cycle_hb_list(void)
{
    struct object * ob;
    if (!hb_list)
	fatal("Cycle heart beat list with empty list!");
    if (hb_list == hb_tail)
	return; /* 1 object on list */
    ob = hb_list;
    hb_list = hb_list -> next_heart_beat;
    hb_tail -> next_heart_beat = ob;
    hb_tail = ob;
    ob->next_heart_beat = 0;
}

/*
 * add or remove an object from the heart beat list; does the major check...
 * If an object removes something from the list from within a heart beat,
 * various pointers in call_heart_beat could be stuffed, so we must
 * check current_heart_beat and adjust pointers.
 */

int set_heart_beat(struct object * ob, int to)
{
    struct object * o = hb_list;
    struct object * oprev = 0;

    if (to)
	to = 1;

    while (o && o != ob) {
	if (!o->enable_heart_beat)
	    fatal("Found disabled object in the active heart beat list!\n");
	oprev = o;
	o = o->next_heart_beat;
	}

    if (!o && ob->enable_heart_beat)
	fatal("Couldn't find enabled object in heart beat list!");
    
    if (to == ob->enable_heart_beat)
	return(0);

    ob->enable_heart_beat = to;

    if (to) {
	if (ob->next_heart_beat)
	    fatal("Dangling pointer to next_heart_beat in object!");
	ob->next_heart_beat = hb_list;
	hb_list = ob;
	if (!hb_tail) hb_tail = ob;
	num_hb_objs++;
    }
    else { /* remove all refs */
	if (hb_list == ob)
	    hb_list = ob->next_heart_beat;
	if (hb_tail == ob)
	    hb_tail = oprev;
	if (oprev)
	    oprev->next_heart_beat = ob->next_heart_beat;
	ob->next_heart_beat = 0;
	num_hb_objs--;
	}

    return(1);
}
/*
 * sigh.  Another status function.
 */
int heart_beat_status() {
    char buf[20];
    add_message("Heart beat:  objects: %d, starts: %d\n",
		num_hb_objs, num_hb_calls);
    sprintf(buf, "%.2f", perc_hb_probes);
    add_message("Percentage of HB calls completed last time: %s\n", buf);
    return 0;
}

/*
 * There is a file with a list of objects to be initialized at
 * start up.
 */

void load_first_objects(void) {
    FILE *f;
    char buff[1000];
    char *p;
    extern int e_flag;
    struct tms tms1, tms2;

    if (!load_object("room/void"))
	fatal("Could not load the void.\n");
    if (e_flag)
	return;
    f = fopen(INIT_FILE, "r");
    if (f == 0)
	return;
    if (setjmp(error_recovery_context)) {
	clear_state();
	add_message("Anomaly in the fabric of world space.\n");
    }
    error_recovery_context_exists = 1;
    times(&tms1);
    while(1) {
	if (fgets(buff, sizeof buff, f) == NULL)
	    break;
	if (buff[0] == '#')
	    continue;
	p = strchr(buff, '\n');
	if (p != 0)
	    *p = 0;
	if (buff[0] == '\0')
	    continue;
	(void)printf("Preloading: %s", buff);
	fflush(stdout);
	eval_cost = 0;
	(void)load_object(buff);
	free_all_values();
#ifdef MALLOC_malloc
	resort_free_list();
#endif
	times(&tms2);
	(void)printf(" %.2f\n", (tms2.tms_utime - tms1.tms_utime +
				 tms2.tms_stime - tms1.tms_stime) / 60.0);
	fflush(stdout);
	tms1 = tms2;
    }
    error_recovery_context_exists = 0;
    fclose(f);
}

/*
 * catch alarm, set flag for comms code and heart_beat to catch.
 * comms code sets time_to_call_heart_beat for the backend when
 * it has completed the current round of player commands.
 */

void catch_alarm(int sig) {
    (void)sig;
    comm_time_to_call_heart_beat = 1;
}

/* This variable is used only for debugging. */
struct object *prev_reset;
/*
 * Move reset to next object, one step for every call of this
 * routine.
 * Beware that the current_reset object might have been destructed
 * by destruct_object(), but then, it was moved to the next object.
 * Every time all objects has been reset, wait for a period until
 * starting next reset cycle.
 */
void move_current_reset(void) {
    static long delay = 0;
    struct value arg;
    jmp_buf save_error_recovery_context;
    int save_rec_exists;
    struct object *ob;
    
    delay--;
    if (delay > time(0l))
	return;
    /*
     * Store the object to reset in a local variable, and move current_reset
     * to the next object that should be reset.
     */
    if (current_reset == 0) {
	prev_reset = 0;
	current_reset = obj_list;
        delay = 30 - num_player;	/* x 2 == minutes. */
	if (delay < 0)
	    delay = 0;
	delay = time(0l) + 60 * delay;	/* Converted to seconds */
	wiz_decay();
	save_wiz_file();
	return;
    }
    ob = current_reset;
    if (ob == 0)		/* Not likely, but possible */
	return;
    /*
     * It can still happen that ob is destructed. This should not be
     * possible, as current_reset never is allowed to point to a destructed
     * object.
     */
    if (ob->destructed)
	fatal("move_current_reset: destructed object 0x%x\n", ob);
    prev_reset = current_reset;
    current_reset = current_reset->next_all;
    if (current_reset && current_reset->destructed)
	fatal("move_current_reset: current_reset points to destructed ob.\n");
#if 0
    if (current_reset)
	printf("current_reset: %s\n", current_reset->name);
    else
	printf("current_reset = 0\n");
#endif
    arg.type = T_NUMBER;
    arg.u.number = 1;
    memcpy((char *) save_error_recovery_context,
	   (char *) error_recovery_context, sizeof error_recovery_context);
    save_rec_exists = error_recovery_context_exists;
    error_recovery_context_exists = 1;
    /* We have to catch an error here, locally.
     * It is not good if the error is catched globally, as the current
     * reset wouldn't be moved forward.
     */
    if (setjmp(error_recovery_context)) {
	clear_state();
	/* The object can have been destructed before the error. */
	if (!ob->destructed)
	    ob->reset = 0;
	if (current_reset && current_reset->destructed)
	    fatal("current_reset pointing to destructed object.\n");
    } else {
	int save_not_touched = ob->not_touched;
#if 0
	/* Reset is no longer called directly, but when the object
	 * is used next time
	 */
        if (ob->reset && ob->not_touched == 0 && ob->swapped == 0)
	    (void)apply("reset", ob, &arg);
#endif
	if (!ob->destructed) {
	    ob->not_touched = save_not_touched + 1;
#if NUM_RESET_TO_SWAP > 0
	    if (ob->not_touched >= NUM_RESET_TO_SWAP && !ob->swapped)
		swap(ob);
#endif
	}
	if (current_reset && current_reset->destructed)
	    fatal("current_reset destructed.\n");
    }
    memcpy((char *) error_recovery_context,
	   (char *) save_error_recovery_context,
	   sizeof error_recovery_context);
    error_recovery_context_exists = save_rec_exists;
}

/*
 * All destructed objects are moved int a sperate linked list,
 * and deallocated after program execution.
 */
void remove_destructed_objects()
{
    struct object *ob, *next;
    for (ob=obj_list_destruct; ob; ob = next) {
	next = ob->next_all;
	destruct2(ob);
    }
    obj_list_destruct = 0;
}

void write_file(char *file, char *str)
{
    FILE *f;

    file = check_file_name(file, 1);
    if (!file)
	return;
    f = fopen(file, "a");
    if (f == 0)
	return;
    fwrite(str, strlen(str), 1, f);
    fclose(f);
    return;
}

int file_size(char *file)
{
    struct stat st;

    file = check_file_name(file, 0);
    if (!file)
	return -1;
    if (stat(file, &st) == -1)
	return -1;
    if (S_IFDIR & st.st_mode)
	return -2;
    return st.st_size;
}

static double load_av = 0.0;

void update_load_av() {
    extern double consts[5];
    extern int current_time;
    static int last_time;
    int n;
    double c;
    static int acc = 0;

    acc++;
    if (current_time == last_time)
	return;
    n = current_time - last_time;
    if ((size_t)n < sizeof consts / sizeof consts[0])
	c = consts[n];
    else
	c = exp(- n / 900.0);
    load_av = c * load_av + acc * (1 - c) / n;
    last_time = current_time;
    acc = 0;
}

static double compile_av = 0.0;

void update_compile_av(int lines)
{
    extern double consts[5];
    extern int current_time;
    static int last_time;
    int n;
    double c;
    static int acc = 0;

    acc += lines;
    if (current_time == last_time)
	return;
    n = current_time - last_time;
    if ((size_t)n < sizeof consts / sizeof consts[0])
	c = consts[n];
    else
	c = exp(- n / 900.0);
    compile_av = c * compile_av + acc * (1 - c) / n;
    last_time = current_time;
    acc = 0;
}

struct value *query_load_av() {
    char buff[100];

    sprintf(buff, "%.2f cmds/s, %.2f comp lines/s", load_av, compile_av);
    return make_string(buff);
}
