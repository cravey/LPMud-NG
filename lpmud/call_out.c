#include <setjmp.h>
#include <string.h>
#include "lint.h"
#include "interpret.h"
#include "object.h"

/*
 * This file implements delayed calls of functions.
 * Static functions can not be called this way.
 *
 * Allocate the structures several in one chunk, to get rid of malloc
 * overhead.
 */

#define CHUNK_SIZE	20

extern char *xalloc PROT((size_t)), *string_copy PROT((char *));
extern jmp_buf error_recovery_context;
extern int error_recovery_context_exists;

struct call {
    int delta;
    char *function;
    struct object *ob;
    struct svalue v;
    struct call *next;
};

static struct call *call_list, *call_list_free;
static int num_call;

/*
 * Free a call out structure.
 */
static void free_call(struct call *cop) {
    free_svalue(&cop->v);
    if (cop->ob) {
	free_object(cop->ob, "free_call");
	cop->ob = 0;
    }
    cop->next = call_list_free;
    free(cop->function);
    cop->function = 0;
    call_list_free = cop;
}

/*
 * Setup a new call out.
 */
void new_call_out(struct object *ob, char *fun, int delay, struct value *arg) {
    struct call *cop, **copp;

    if (delay < 0)
	delay = 0;
    if (!call_list_free) {
	int i;
	call_list_free =
	    (struct call *)xalloc(CHUNK_SIZE * sizeof (struct call));
	for (i=0; i<CHUNK_SIZE - 1; i++)
	    call_list_free[i].next  = &call_list_free[i+1];
	call_list_free[CHUNK_SIZE-1].next = 0;
	num_call += CHUNK_SIZE;
    }
    cop = call_list_free;
    call_list_free = call_list_free->next;
    cop->function = string_copy(fun);
    cop->ob = ob;
    add_ref(ob, "call_out");
    cop->v.type = T_NUMBER;
    cop->v.u.number = 0;
    if (arg)
	assign_svalue(&cop->v, arg);
    for (copp = &call_list; *copp; copp = &(*copp)->next) {
	if ((*copp)->delta >= delay) {
	    (*copp)->delta -= delay;
	    cop->delta = delay;
	    cop->next = *copp;
	    *copp = cop;
	    return;
	}
	delay -= (*copp)->delta;
    }
    *copp = cop;
    cop->delta = delay;
    cop->next = 0;
}

/*
 * See if there are any call outs to be called.
 */
void call_out() {
    struct call *cop;
    jmp_buf save_error_recovery_context;
    int save_rec_exists;
    struct object *save_command_giver;
    extern struct object *command_giver;
    extern int current_time;
    static int last_time;

    if (call_list == 0) {
	last_time = current_time;
	return;
    }
    if (last_time == 0)
	last_time = current_time;
    save_command_giver = command_giver;
    call_list->delta -= current_time - last_time;
    last_time = current_time;
    memcpy((char *) save_error_recovery_context,
	   (char *) error_recovery_context, sizeof error_recovery_context);
    save_rec_exists = error_recovery_context_exists;
    error_recovery_context_exists = 1;
    while (call_list && call_list->delta <= 0) {
	/*
	 * Move the first call_out out of the chain.
	 */
	cop = call_list;
	call_list = call_list->next;
	/*
	 * A special case:
	 * If a lot of time has passed, so that current call out was missed,
	 * then it will have a negative delta. This negative delta implies
	 * that the next call out in the list has to be adjusted.
	 */
	if (call_list && cop->delta < 0)
	    call_list->delta += cop->delta;
	if (!cop->ob->destructed) {
	    /* We have to catch an error here, locally.
	     * It is not good if the error is catched globally, as the current
	     * call out wouldn't be removed.
	     */
	    if (setjmp(error_recovery_context)) {
		extern void clear_state();
		clear_state();
		debug_message("Error in call out.\n");
	    } else {
		struct value v;
		extern struct value const0;
		
		if (cop->ob->enable_commands)
		    command_giver = cop->ob;
		else
		    command_giver = 0;
		v.type = cop->v.type;
		v.u = cop->v.u;
		if (v.type == T_OBJECT && v.u.ob->destructed) {
		    v.type = T_NUMBER;
		    v.u.number = 0;
		}
		apply(cop->function, cop->ob, &v);
	    }
	}
	free_call(cop);
    }
    memcpy((char *) error_recovery_context,
	   (char *) save_error_recovery_context,
	   sizeof error_recovery_context);
    error_recovery_context_exists = save_rec_exists;
    command_giver = save_command_giver;
}

/*
 * Throw away a call out. First call to this function is discarded.
 * The time left until execution is returned.
 * -1 is returned if no call out pending.
 */
int remove_call_out(struct object *ob, char *fun) {
    struct call **copp, *cop;
    int delay = 0;

    for (copp = &call_list; *copp; copp = &(*copp)->next) {
	delay += (*copp)->delta;
	if ((*copp)->ob == ob && strcmp((*copp)->function, fun) == 0) {
	    cop = *copp;
	    if (cop->next)
		cop->next->delta += cop->delta;
	    *copp = cop->next;
	    free_call(cop);
	    return delay;
	}
    }
    return -1;
}

int print_call_out_usage() {
    int i;
    struct call *cop;

    for (i=0, cop = call_list; cop; cop = cop->next)
	i++;
    add_message("call out      %6d %6d (length %d)\n", num_call,
		num_call * sizeof (struct call), i);
    return num_call * sizeof (struct call);
}
