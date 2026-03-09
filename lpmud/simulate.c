#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include "lint.h"
#include "config.h"
#include "stdio.h"
#include "lnode.h"
#include "y.tab.h"
#include "interpret.h"
#include "object.h"
#include "sent.h"
#include "wiz_list.h"

char *inherit_file;

extern char *xalloc PROT((size_t)), *string_copy PROT((char *)),
    *dump_trace PROT((void)),
    *check_file_name PROT((char *, int));

char *last_verb;

extern int special_parse PROT((char *)), yyparse(),
    set_call PROT((struct object *, struct sentence *, int)),
    parse_command PROT((char *, struct object *)),
    legal_path PROT((char *)), print_call_out_usage();

void pre_compile PROT((char *)),
    remove_interactive PROT((struct object *)),
    add_light PROT((struct object *, int)),
    do_write PROT((struct value *)), add_action PROT((char *, char *, int)),
    add_verb PROT((char *)),
    print_local_commands(), ipc_remove(),
    show_info_about PROT((char *, char *, struct interactive *)),
    ed_start PROT((char *)), say PROT((struct value *, struct object *)),
    set_snoop PROT((struct object *, struct object *)),
    tell_room PROT((struct object *, struct value *)),
    print_lnode_status PROT((int)),
    remove_all_players(), start_new_file PROT((FILE *)),
    move_or_destruct PROT((struct object *, struct object *)),
    startshutdowngame PROT((int)),
    load_ob_from_swap PROT((struct object *)), dump_malloc_data(),
    dump_all_objects(),
    debug_message_value();

extern int d_flag;

struct object *obj_list, *obj_list_destruct;

extern int current_line;

struct object *current_object;	/* The object interpreting a function. */
struct object *command_giver;	/* Where the current command came from. */

int num_parse_error;		/* Number of errors in the parser. */

void shutdowngame();

struct svalue *find_value(struct lnode_variable *p)
{
    if (p->number >= current_object->num_variables) {
	dump_trace();
	fatal("Illegal variable access %d(%d). See trace above.\n",
	    p->number, current_object->num_variables);
    }
    return &current_object->variables[p->number];
}

struct lnode_var_def *find_status(char *str, int must_find)
{
    struct lnode_var_def *p;

    for (p=current_object->status; p; p = p->next) {
	if (strcmp(p->name, str) == 0)
	    return p;
    }
    if (!must_find)
	return 0;
    error("--Status %s not found in prog for %s\n", str,
	   current_object->name);
    return 0;
}

/*
 * Save the name of the current .i file, so we are able to remove it
 * if it contains illegal include files.
 */
char *current_i_file;

void remove_i_file() {
    unlink(current_i_file);
}

/*
 * Load an object definition from file. If the object wants to inherit
 * from an object that is not loaded, discard all, load the inherited object,
 * and reload again.
 *
 * Save the command_giver, because reset() in the new object might change
 * it.
 */
struct object *load_object(char *lname)
{
    FILE *f;
    extern int total_lines;

    struct object *ob, *save_command_giver = command_giver;
    extern struct lnode *prog, *heart_beat;
    extern char *current_file;
    extern int variable_count, static_variable_flag;
    extern struct object *inherit_ob;
    struct stat i_st, c_st;
    int name_length;
    char real_name[200], name[200];

    /* Remove leading '/' if any. */
    while(lname[0] == '/')
	lname++;
    /*
     * We must clear inherit_ob, to be sure it is initialized to 0.
     * It can still be pointing to something, where an error aborted
     * execution.
     */
    inherit_ob = 0;
    /* Truncate possible .c in the object name. */
    strncpy(name, lname, sizeof(name) - 1);
    name[sizeof(name) - 1] = '\0';
    name_length = strlen(name);
    if ((size_t)name_length > sizeof name - 4)
	name_length = sizeof name - 4;
    name[name_length] = '\0';
    if (name_length >= 2 &&
	name[name_length-2] == '.' && name[name_length-1] == 'c') {
	name[name_length-2] = '\0';
	name_length -= 2;
    }
    /*
     * First check that the c-file exists.
     */
    (void)strcpy(real_name, name);
    (void)strcat(real_name, ".c");
    if (stat(real_name, &c_st) == -1) {
	fprintf(stderr, "Could not load descr for %s\n", real_name);
	error("Failed to load file.\n");
	return 0;
    }
    /*
     * Check if it's a legal name.
     */
    if (!legal_path(real_name)) {
	fprintf(stderr, "Illegal pathname: %s\n", real_name);
	error("Illegal path name.\n");
	return 0;
    }
    /*
     * Now check if the i-file is newer or nonexisting.
     */
    real_name[name_length+1] = 'i';
    if (stat(real_name, &i_st) == -1 || i_st.st_mtime < c_st.st_mtime) {
	real_name[name_length+1] = 'c';
	pre_compile(name);
	real_name[name_length+1] = 'i';
    }
    if (stat(real_name, &i_st) == -1 || i_st.st_mtime < c_st.st_mtime) {
        if (command_giver)
	    add_message("Failed to compile the new file.\n");
    }
    current_i_file = real_name;
    f = fopen(real_name, "r");
    if (f == 0) {
	perror(real_name);
	error("Could not read the file.\n");
    }
    start_new_file(f);
    current_file = string_copy(real_name);	/* This one is freed below */
    current_line = 0;
    prog = heart_beat = 0;
    prog_status = 0;
    variable_count = 0;
    num_parse_error = 0;
    static_variable_flag = 0;
    (void)yyparse();
    update_compile_av(total_lines);
    total_lines = 0;
    (void)fclose(f);
#ifdef REMOVE_I_FILES
    unlink(current_i_file);
#else
    /*
     * Remove the i-file if there was an error. This
     * is necessary, to protect against security problems.
     */
    if (num_parse_error)
	unlink(current_i_file);
#endif
    free(current_file);
    current_file = 0;
    current_i_file = 0;
    if (inherit_file == 0 && (num_parse_error > 0 || prog == 0)) {
	if (prog) {
	    /*
	     * Set the reference count to one.
	     * We don't want to confuse free_prog().
	     */
	    add_prog_ref((struct lnode_def *)prog);
	    if (!free_prog((struct lnode_def *)prog, 0, 0))
		fatal("Failed to release prog area.\n");
	}
	if (num_parse_error == 0 && prog == 0)
	    error("No program in object !\n");
	error("Error in loading object\n");
    }
    if (inherit_file) {
	char *tmp = inherit_file;
	if (prog) {
	    /*
	     * Set the reference count to one.
	     * We don't want to confuse free_prog().
	     */
	    add_prog_ref((struct lnode_def *)prog);
	    if (!free_prog((struct lnode_def *)prog, 0, 0))
		fatal("Failed to release prog area.\n");
	    prog = 0;
	}
	if (strcmp(inherit_file, name) == 0) {
	    free(inherit_file);
	    inherit_file = 0;
	    error("Illegal to inherit self.\n");
	}
	inherit_file = 0;
	load_object(tmp);
	free(tmp);
	ob = load_object(name);
	return ob;
    }
    ob = get_empty_object(variable_count);
    if (inherit_ob) {
	if (strcmp(inherit_ob->name, name) == 0) {
	    free((char *)ob);
	    inherit_ob = 0;
	    error("Illegal to inherit self.\n");
	}
	ob->inherit = inherit_ob;
	inherit_ob->inherited = 1;
	add_ref(inherit_ob, "load_object inherit");
	if (ob->heart_beat == 0)
	    ob->heart_beat = inherit_ob->heart_beat;
	inherit_ob = 0;
    }
    ob->name = string_copy(name);
    ob->name_length = strlen(name);
    /* Is this object wizard defined ? */
    {
        char wiz_name[100];
	if (sscanf(real_name, "players/%99s", wiz_name) == 1) {
	    char *np;
	    np = strchr(wiz_name, '/');
	    if (np)
		*np = '\0';
	    ob->wl = add_name(wiz_name);
	} else {
	    ob->wl = 0;
	}
    }
#if 0
    /* No longer here. It is done in get_empty_object() */
    /* Allocate space for local variables. */
    if (variable_count > 0)
	ob->variables =
	    (struct svalue *)xalloc(variable_count * sizeof (struct svalue));
    ob->num_variables = variable_count;
    /* Initialize variables... */
    for (i=0; i<variable_count; i++) {
	ob->variables[i].u.number = 0;
	ob->variables[i].type = T_NUMBER;
    }
#endif
    ob->status = prog_status;
    if (prog) {
	ob->prog = (struct lnode_def *)prog;
	add_prog_ref((struct lnode_def *)prog);
    } else {
	ob->prog = 0;
    }
    ob->next_all = obj_list;
    obj_list = ob;
    if (heart_beat)
	ob->heart_beat = heart_beat;
    enter_object_hash(ob);	/* add name to fast object lookup table */
    ob->reset = 1;
    (void)apply("reset", ob, 0);
    command_giver = save_command_giver;
    if (d_flag)
	debug_message("--%s loaded\n", ob->name);
#if NUM_RESET_TO_SWAP > 0
    swap(ob);
#endif
    return ob;
}

struct object *previous_ob;
extern struct lnode_2 *next_arg_list_to_use;

/*
 * Call function in another object. The object can be given both as a string
 * and a pointer.
 *
 * Save the caller object in previous_ob, so some functions can
 * see who called them.
 *
 * If find_object() is called, the object might be non-existing, and
 * thus loaded. If the object is loaded, "reset" will be called in the object.
 * next_arg_list_to_use must be hidden, so that the reset() won't
 * get these arguments.
 */
struct value *call_other(struct value *obj, char *name, struct value *arg)
{
    struct object *ob, *save_command_giver = command_giver;
    struct value *ret;

    if (obj->type == T_OBJECT) {
	ob = obj->u.ob;
    } else {
	struct lnode_2 *save_args = next_arg_list_to_use;
	next_arg_list_to_use = 0;
	ob = find_object(obj->u.string);
	next_arg_list_to_use = save_args;
    }
    if (ob == 0)
	error("Could not find object %s\n", obj);
    /*
     * Test if this really is an existing object.
     */
    if (ob->reset == 0) {
	next_arg_list_to_use = 0;
	return &const0;
    }
    if (ob->not_touched) {
	struct lnode_2 *save_args = next_arg_list_to_use;
	next_arg_list_to_use = 0;
	ob->not_touched = 0;
	(void)apply("reset", ob, &const1);
	next_arg_list_to_use = save_args;
	command_giver = save_command_giver;
	if (ob->destructed) {
	    next_arg_list_to_use = 0;
	    return &const0;
	}
    }
    ret = apply(name, ob, arg);
    command_giver = save_command_giver;
    if (ret == 0)
	return &const0;
    return ret;
}

struct value *this_player() {
    struct value *p;

    if (command_giver == 0 || command_giver->destructed)
	return &const0;
    p = alloc_value();
    p->type = T_OBJECT;
    p->u.ob = command_giver;
    add_ref(command_giver, "this_player()");
    return p;
}

struct value *this_object() {
    struct value *p;

    if (current_object->destructed)
	return &const0;
    p = alloc_value();
    p->type = T_OBJECT;
    p->u.ob = current_object;
    add_ref(current_object, "this_object()");
    return p;
}

struct value *call_local_function(char *name, struct value *arg)
{
    struct value *ret;

    ret = apply(name, current_object, arg);
    if (ret == 0)
	error("Local function %s not found.\n", name);
    return ret;
}

char *make_new_name(char *str)
{
    static int i;
    char *p = xalloc(strlen(str) + 10);

    (void)sprintf(p, "%s#%d", str, i);
    i++;
    return p;
}

/*
 *
 * Save the command_giver, because reset() in the new object might change
 * it.
 */
struct value *clone_object(char *str1)
{
    struct object *ob, *new_ob;
    struct value *ret;
    struct object *save_command_giver = command_giver;

    ob = find_object(str1);
    if (!ob) {
	if (strcmp(str1, "obj/sing") == 0)
	    fatal("No singularity.\n");
	ret = clone_object("obj/sing");
	if (!ret)
	    fatal("No singularity.\n");
	return ret;
    }
    if (ob->super || ob->cloned)
	error("Cloning a used object !\n");
    if (ob->enable_heart_beat)
	(void)set_heart_beat(ob, 0);
    ob->reset = 0;
    new_ob = get_empty_object(ob->num_variables);
    new_ob->name = make_new_name(ob->name);
    new_ob->name_length = strlen(new_ob->name);
    new_ob->cloned = 1;
    new_ob->prog = ob->prog;
    new_ob->inherit = ob->inherit;
    if (new_ob->inherit)
	add_ref(new_ob->inherit, 0);
    if (new_ob->prog)
	add_prog_ref((struct lnode_def *)(new_ob->prog));
    if (current_object && current_object->wl && !ob->wl)
	new_ob->wl = current_object->wl;
    else
	new_ob->wl = ob->wl;		/* Possibly a null pointer */
    new_ob->status = ob->status;
    new_ob->heart_beat = ob->heart_beat;
    new_ob->next_all = obj_list;
    new_ob->block = ob->block;
    new_ob->swap_size = ob->swap_size;	/* Used only for statistics. */
    obj_list = new_ob;
    enter_object_hash(new_ob);	/* add name to fast object lookup table */
#if 0	/* This was done in get_empty_object() */
    new_ob->num_variables = ob->num_variables;
    /* Allocate space for local variables. */
    if (ob->num_variables > 0)
	new_ob->variables =
	    (struct svalue *)xalloc(ob->num_variables *
				    sizeof (struct svalue));
    /* Initialize variables... */
    for (i=0; i<ob->num_variables; i++) {
	new_ob->variables[i].u.number = 0;
	new_ob->variables[i].type = T_NUMBER;
    }
#endif
    new_ob->reset = 1;
    (void)apply("reset", new_ob, 0);
    command_giver = save_command_giver;
    if (d_flag)
	debug_message("--%s cloned to %s\n", ob->name, new_ob->name);
    if (new_ob->destructed)
	return &const0;
    ret = alloc_value();
    ret->type = T_OBJECT;
    ret->u.ob = new_ob;
    add_ref(new_ob, "clone_object");
    return ret;
}

struct value *environment(struct value *arg)
{
    struct value *ret;
    struct object *ob;

    if (arg && arg->type == T_OBJECT)
	ob = arg->u.ob;
    else if (arg && arg->type == T_STRING)
	ob = find_object2(arg->u.string);
    else
	ob = current_object;
    if (ob == 0 || ob->super == 0)
	return &const0;
    if (ob->destructed)
	error("environment() off destructed object.\n");
    ret = alloc_value();
    ret->type = T_OBJECT;
    ret->u.ob = ob->super;
    add_ref(ret->u.ob, "environment");
    return ret;
}

/*
 * Execute a command for an object. Copy the command into a
 * new buffer, because 'parse_command()' can modify the command.
 * If the object is not current object, static functions will not
 * be executed. This will prevent forcing players to do illegal things.
 */
int command_for_object(char *str, struct object *ob)
{
    char buff[1000];

    if (strlen(str) > sizeof(buff) - 1)
	error("Too long command.\n");
#ifdef LOG_FORCE
    if (ob && command_giver != ob && ob->interactive) {
	struct value *force, *forced;
	FILE *f;

	force = apply("query_real_name", command_giver, 0);
	forced = apply("query_real_name", ob, 0);
	f = fopen("log/FORCE", "a");
	if (f && force && forced) {
	    if (force->type == T_STRING && forced->type == T_STRING)
		fprintf(f, "%s %s: %s\n", force->u.string, forced->u.string,
			str);
	}
	if (f)
	    fclose(f);
    }
#endif
    if (ob == 0)
	ob = current_object;
    strncpy(buff, str, sizeof buff);
    return parse_command(buff, ob);
}

/*
 * To find if an object is present, we have to look in two inventory
 * lists. The first list is the inventory of the current object.
 * The second list is all things that have the same ->super as
 * current_object.
 * Also test the environment.
 * If the second argument 'ob' is non zero, only search in the
 * inventory of 'ob'. The argument 'ob' will be mandatory, later.
 */

static struct value *object_present2(struct value *v, struct object *ob);

struct value *object_present(struct value *v, struct object *ob)
{
    struct value *ret;
    int specific = 0;

    if (ob == 0)
	ob = current_object;
    else
	specific = 1;
    if (v->type == T_OBJECT) {
	if (specific) {
	    if (v->u.ob->super == ob)
		return v;
	    else
		return &const0;
	}
	if (v->u.ob->super == ob ||
	    (v->u.ob->super == ob->super && ob->super != 0))
	    return v;
	return &const0;
    }
    ret = object_present2(v, ob->contains);
    if (ret)
	return ret;
    if (specific)
	return &const0;
    if (ob->super) {
	ret = apply("id", ob->super, v);
	if (ob->super->destructed)
	    return &const0;
	if (ret && !(ret->type == T_NUMBER && ret->u.number == 0)) {
	    ret = alloc_value();
	    ret->type = T_OBJECT;
	    ret->u.ob = ob->super;
	    add_ref(ret->u.ob, "present()");
	    return ret;
	}
	return object_present2(v, ob->super->contains);
    }
    return &const0;
}

static struct value *object_present2(struct value *v, struct object *ob)
{
    struct value *ret;
    char *p;
    int count = 0, length;
    struct value item;

    item.type = T_STRING;
    item.u.string = string_copy(v->u.string);
    length = strlen(item.u.string);
    p = item.u.string + length - 1;
    if (*p >= '0' && *p <= '9') {
	while(p > item.u.string && *p >= '0' && *p <= '9')
	    p--;
	if (p > item.u.string && *p == ' ') {
	    count = atoi(p+1) - 1;
	    *p = '\0';
	}
    }
    for (; ob; ob = ob->next_inv) {
	ret = apply("id", ob, &item);
	if (ob->destructed) {
	    free(item.u.string);
	    return 0;
	}
	if (ret == 0 || (ret->type == T_NUMBER && ret->u.number == 0))
	    continue;
	if (count-- > 0)
	    continue;
	ret = alloc_value();
	ret->type = T_OBJECT;
	ret->u.ob = ob;
	add_ref(ret->u.ob, "object_present2");
	free(item.u.string);
	return ret;
    }
    free(item.u.string);
    return 0;
}

/*
 * Remove an object. It is first moved into the destruct list, and
 * not really destructed until later. (destruct2()).
 *
 * If the object is inherited, it must not be destructed. So we just rename
 * it to something new. Thus will all pointers to this object continue to
 * work, and new objects that needs this object will load a new copy.
 */
void destruct_object(struct value *v)
{
    struct object *ob, *super;
    struct object **pp;
    int removed;
    struct value *weight, neg_weight;
    extern struct object *current_reset;

    if (v->type == T_OBJECT)
	ob = v->u.ob;
    else {
	ob = find_object2(v->u.string);
	if (ob == 0)
	    error("destruct_object: Could not find %s\n", v->u.string);
    }
    if (ob->destructed)
	error("Destruct destructed object.\n");
    if (d_flag)
	debug_message("Destruct object %s (ref %d)\n", ob->name, ob->ref);
    /*
     * Inherited objects will never be removed.
     */
    if (ob->inherited) {
	char *p;
	remove_object_hash(ob);
	p = ob->name;
	ob->name = make_new_name(p);
	free(p);
	ob->name_length = strlen(ob->name);
	enter_object_hash(ob);
	return;
    }
    if (ob == current_reset) {
	/*
	 * current_reset must be set to 0, not the next object in the list,
	 * as this next object migh be destructed from a recursive call of
	 * us from destruct_object()_ from above !
	 */
	current_reset = 0;
    }
    super = ob->super;
    if (super == 0) {
	super = find_object("room/void");	/* Take any existing void. */
    } else {
	/* Call exit in current room, if player or npc */
	if(ob->enable_commands) {
	    struct value v;
	    v.type = T_OBJECT;
	    v.u.ob = ob;
	    apply("exit",super,&v);
	}
	weight = apply("query_weight", ob, 0);
	if (weight && weight->type == T_NUMBER) {
	    neg_weight.type = T_NUMBER;
	    neg_weight.u.number = - weight->u.number;
	    (void)apply("add_weight", super, &neg_weight);
	}
    }
    if (super == 0)
	fatal("Could not find the void.\n");
    while(ob->contains)
	move_or_destruct(ob->contains, super);
    set_heart_beat(ob, 0);
    /*
     * Remove us out of this current room (if any).
     * Remove all sentences defined by this object from all objects here.
     */
    if (ob->super) {
	if (ob->super->enable_commands)
	    remove_sent(ob, ob->super);
	add_light(ob->super, - ob->total_light);
	for (pp = &ob->super->contains; *pp;) {
	    if ((*pp)->enable_commands)
		remove_sent(ob, *pp);
	    if (*pp != ob)
		pp = &(*pp)->next_inv;
	    else
		*pp = (*pp)->next_inv;
	}
    }
    if (ob->inherit) {
	struct object *tmp = ob->inherit;
	ob->inherit = 0;
	free_object(tmp, "inherit");
    }
    /*
     * Now remove us out of the list of all objects.
     * This must be done last, because an error in the above code would
     * halt execution.
     */
    removed = 0;
    for (pp = &obj_list; *pp; pp = &(*pp)->next_all) {
	if (*pp != ob)
	    continue;
	*pp = (*pp)->next_all;
	removed = 1;
	remove_object_hash(ob);
	break;
    }
    if (!removed)
        fatal("Failed to delete object.\n");
    if (ob->living_name)
	remove_living_name(ob);
    ob->super = 0;
    ob->next_inv = 0;
    ob->heart_beat = 0;
    ob->contains = 0;
    ob->enable_commands = 0;
    ob->next_all = obj_list_destruct;
    obj_list_destruct = ob;
    ob->destructed = 1;
}

/*
 * This one is called when no program is execuiting.
 * The super pointer is still maintained.
 */
void destruct2(struct object *ob)
{
    if (ob->interactive)
	remove_interactive(ob);
    /*
     * We must deallocate variables here, not in 'free_object()'.
     * That is because one of the local variables may point to this object,
     * and deallocation of this pointer will also decrease the reference
     * count of this object. Otherwise, an object with a variable pointing
     * to itself, would never be freed.
     * Just in case the program in this object would continue to
     * execute, change string and object variables into the number 0.
     */
    if (ob->num_variables > 0) {
	/*
	 * Deallocate variables in this object.
	 * The space of the variables are not deallocated until
	 * the object structure is freed in free_object().
	 */
	int i;
	for (i=0; i<ob->num_variables; i++) {
	    free_svalue(&ob->variables[i]);
	    ob->variables[i].type = T_NUMBER;
	    ob->variables[i].u.number = 0;
	}
    }
    free_object(ob, "destruct_object");
}

struct value *create_wizard(char *owner)
{
    struct stat st;
    char name[200];
    FILE *f, *in;
    struct value *ret;
    struct object *owner_obj;
    char line[512];
    const char *p;

    if (strcmp(current_object->name, "room/port_castle") != 0) {
	error("Illegal use of create_wizard()\n");
    }
    if (owner == 0 || owner[0] == '\0')
	error("Bad name to create_wizard: %s\n", owner);
    for (p = owner; *p; p++) {
	if (*p < 'a' || *p > 'z')
	    error("Bad name to create_wizard: %s\n", owner);
    }
    owner_obj = find_living_object(owner, 1);
    if (owner_obj == 0) {
	fprintf(stderr,
		"create_wizard: Could not find living object %s.\n", owner);
	return 0;
    }
    if (owner_obj->super == 0 || owner_obj->super->cloned) {
	struct value v;
	v.type = T_STRING;
	v.u.string = 
	  "There is a crash, the room collapses and the castle disappears.\n";
	say(&v, 0);
	return 0;
    }
    if (stat(PLAYER_DIR, &st) == -1)
	if (mkdir(PLAYER_DIR, 0777) == -1)
	    error("Could not mkdir %s\n", PLAYER_DIR);
    (void)sprintf(name, "%s/%s", PLAYER_DIR, owner);
    if (stat(name, &st) == 0)
	error("Player %s already has a castle!\n", owner);
    if (mkdir(name, 0777) == -1) {
	perror(name);
	error("Could not mkdir %s\n", name);
    }
    (void)sprintf(name, "%s/%s/%s", PLAYER_DIR, owner, "castle.c");
    f = fopen(name, "w");
    if (f == NULL)
	error("Could not create a castle file %s!\n", name);
    (void)fprintf(f, "#define NAME \"%s\"\n", owner);
    (void)fprintf(f, "#define DEST \"%s\"\n", owner_obj->super->name);
    in = fopen(DEFAULT_CASTLE, "r");
    if (in == NULL) {
	(void)fclose(f);
	error("Could not read default castle file %s!\n", DEFAULT_CASTLE);
    }
    while (fgets(line, sizeof line, in)) {
	if (fputs(line, f) == EOF) {
	    (void)fclose(in);
	    (void)fclose(f);
	    error("Could not append default castle to %s!\n", name);
	}
    }
    (void)fclose(in);
    (void)fclose(f);
    f = fopen(INIT_FILE, "a");
    if (f == NULL)
	error("Could not add the new castle to the %s\n", INIT_FILE);
    (void)fprintf(f, "%s\n", name);
    (void)fclose(f);
    ret = alloc_value();
    ret->type = T_STRING;
    ret->u.string = string_copy(name);
    return ret;
}

static int has_efun_privilege(int min_level, char *name)
{
    struct value *v;

    if (!command_giver || !command_giver->interactive)
	return 0;
    v = apply("query_level", command_giver, 0);
    if (!command_giver || command_giver->destructed || !v ||
	v->type != T_NUMBER || v->u.number < min_level) {
	add_message("Permission denied for %s().\n", name);
	return 0;
    }
    return 1;
}

/*
 * A message from an object will reach
 * all objects in the inventory,
 * all objects in the same environment and
 * the surrounding object.
 * Non interactive objects gets no messages.
 *
 * There are two cases to take care of. If this routine is called from
 * a player (indirectly), then the message goes to all in his
 * environment. Otherwise, the message goes to all in the current_object's
 * environment (as the case when called from a heart_beat()).
 *
 * If there is a second argument 'avoid_ob', then do not send the message
 * to that object.
 */

void say(struct value *v, struct object *avoid_ob)
{
    struct object *ob, *save_command_giver = command_giver;
    struct object *origin;
    char buff[256];

    if (current_object->enable_commands)
	command_giver = current_object;
    if (command_giver)
	origin = command_giver;
    else
	origin = current_object;
    switch(v->type) {
    case T_STRING:
	strncpy(buff, v->u.string, sizeof buff);
	buff[sizeof buff - 1] = '\0';
	break;
    case T_OBJECT:
	strncpy(buff, v->u.ob->name, sizeof buff);
	buff[sizeof buff - 1] = '\0';
	break;
    case T_NUMBER:
	sprintf(buff, "%d", v->u.number);
	break;
    default:
	error("Invalid argument %d to say()\n", v->type);
    }
    for (ob = origin->contains; ob; ob = ob->next_inv) {
	struct object *save_again;
	if (ob->interactive == 0) {
	    if (ob->enable_commands && ob != command_giver && ob != avoid_ob)
		tell_npc(ob, buff);
	    if (ob->destructed)
		break;
	    continue;
	}
	if (ob == save_command_giver || ob == avoid_ob)
	    continue;
	save_again = command_giver;
	command_giver = ob;
	add_message("%s", buff);
	command_giver = save_again;
    }
    if (origin->super) {
	if (origin->super->interactive && origin != command_giver &&
		origin->super != avoid_ob) {
	    command_giver = origin->super;
	    add_message("%s", buff);
	} else if (origin->super->interactive == 0 &&
		   origin->super != avoid_ob &&
		   origin->super->enable_commands && ob != command_giver) {
	    tell_npc(origin->super, buff);
	}
	for (ob = origin->super->contains; ob; ob = ob->next_inv) {
	    struct object *save_again;
	    if (ob == avoid_ob)
		continue;
	    if (ob->interactive == 0) {
		if (ob->enable_commands && ob != command_giver)
		    tell_npc(ob, buff);
		if (ob->interactive)
		    break;
		continue;
	    }
	    if (ob == command_giver)
		continue;
	    save_again = command_giver;
	    command_giver = ob;
	    add_message("%s", buff);
	    command_giver = save_again;
	}
    }
    command_giver = save_command_giver;
}

/*
 * Send a message to all objects inside an object.
 * Non interactive objects gets no messages.
 * Compare with say().
 */

void tell_room(struct object *room, struct value *v)
{
    struct object *ob, *save_command_giver = command_giver;
    char buff[256];

    switch(v->type) {
    case T_STRING:
	strncpy(buff, v->u.string, sizeof buff);
	buff[sizeof buff - 1] = '\0';
	break;
    case T_OBJECT:
	strncpy(buff, v->u.ob->name, sizeof buff);
	buff[sizeof buff - 1] = '\0';
	break;
    case T_NUMBER:
	sprintf(buff, "%d", v->u.number);
	break;
    default:
	error("Invalid argument %d to tell_room()\n", v->type);
    }
    for (ob = room->contains; ob; ob = ob->next_inv) {
	if (ob->interactive == 0) {
	    if (ob->enable_commands)
		tell_npc(ob, buff);
	    if (ob->destructed)
		break;
	    continue;
	}
	command_giver = ob;
	add_message("%s", buff);
    }
    command_giver = save_command_giver;
}

void shout_string(char *str)
{
    struct object *ob, *save_command_giver = command_giver;
    FILE *f = 0;
    char *p;
    struct value *v;

    for (p=str; *p; p++) {
	if ((*p < ' ' || *p > '~') && *p != '\n')
	    *p = ' ';
    }
    p = 0;
#ifdef LOG_SHOUT
    if (command_giver) {
	v = apply("query_real_name", command_giver, 0);
	if (v && v->type == T_STRING)
	    p = v->u.string;
    } else if (current_object && current_object->wl)
	p = current_object->wl->name;
    if (p)
	f = fopen("log/SHOUTS", "a");
    if (f) {
	fprintf(f, "%s: %s\n", p, str);
	fclose(f);
    }
#endif
    for (ob = obj_list; ob; ob = ob->next_all) {
	if (ob->interactive == 0 || ob == save_command_giver)
	    continue;
	command_giver = ob;
	add_message("%s", str);
    }
    command_giver = save_command_giver;
}

struct value *first_inventory(struct value *arg)
{
    struct object *ob;
    struct value *ret;

    if (arg->type == T_STRING)
	ob = find_object(arg->u.string);
    else
	ob = arg->u.ob;
    if (ob == 0)
	error("No object to first_inventory()");
    if (ob->contains == 0)
	return 0;
    ret = alloc_value();
    ret->type = T_OBJECT;
    ret->u.ob = ob->contains;
    add_ref(ret->u.ob, "first_inventory");
    return ret;
}

struct value *next_inventory(struct value *arg)
{
    struct object *ob;
    struct value *ret;

    if (arg->type == T_STRING)
	ob = find_object(arg->u.string);
    else
	ob = arg->u.ob;
    if (ob == 0)
	error("No object to next_inventory()");
    if (ob->next_inv == 0)
	return 0;
    ret = alloc_value();
    ret->type = T_OBJECT;
    ret->u.ob = ob->next_inv;
    add_ref(ret->u.ob, "next_inventory");
    return ret;
}

/*
 * This will enable an object to use commands normally only
 * accessible by interactive players.
 * Also check if the player is a wizard. Wizards must not affect the
 * value of the wizlist ranking.
 */

void enable_commands(int num)
{
    current_object->enable_commands = num;
    command_giver = current_object;
}

/*
 * Set up a function in this object to be called with the next
 * user input string.
 */
struct value *input_to(char *fun, int flag)
{
    struct sentence *s = alloc_sentence();

    if (set_call(command_giver, s, flag)) {
	s->function = make_shared_string(fun);
	s->ob = current_object;
	add_ref(current_object, "input_to");
	return &const1;
    }
    free_sentence(s);
    return &const0;
}

#define MAX_LINES 50

int pstrcmp(const void *p1, const void *p2)
{
    const char *const *s1 = (const char *const *)p1;
    const char *const *s2 = (const char *const *)p2;
    return strcmp(*s1, *s2);
}

/*
 * List files in a directory. The standard 'ls' could be used, but
 * it takes too much time.
 * Prepared for flag decoding.
 *
 * Look at the the last component of the path name. It is a regular
 * expression, select only matching files.
 *
 * All .i files and '.' and '..' are only listed if they match the regexp.
 */
void list_files(char *path, char *flags)
{
    char buff[160];	/* Will never need more than two lines */
    int i, max, lines, num, j, truncated = 0, pos;
    DIR *dirp;
    struct dirent *de;
    char **names, name_buffer[MAX_LINES * 80];
    struct stat st;
    char regexp[80], *p, path2[100];
    int do_match = 0;
    (void)flags;

    if (!path)
        path = ".";
    path = check_file_name(path, 0);
    if (path == 0)
        return;
    strncpy(path2, path, sizeof path2);
    path2[sizeof path2 - 1] = '\0';
    p = path2 + strlen(path2) - 2;
    if (p >= path2 && p[0] == '/' && p[1] == '.')
	p[0] = '\0';
    if (stat(path2, &st) == -1) {
	/* Either the directory does not exist, or it is a regexp
	 * file name. Strip of the last component.
	 */
	p = strrchr(path2, '/');
	if (p == 0 || p[1] == '\0')
	    return;
	strncpy(regexp, p+1, sizeof regexp);
	regexp[sizeof regexp - 1] = '\0';
	*p = '\0';
	do_match = 1;
    }
    dirp = opendir(path2);
    if (dirp == 0) {
	add_message("No such directory '%s'\n", path2);
	return;
    }
    pos = 0;
    max = 0;
    num = 0;
    /*
     * First step, find the number of file names.
     */
    for(de = readdir(dirp); de; de = readdir(dirp)) {
	int dlen = strlen(de->d_name);
	if (!do_match && (strcmp(de->d_name, ".") == 0 ||
			  strcmp(de->d_name, "..") == 0))
	    continue;
	if (!do_match && dlen >= 2 &&
	    de->d_name[dlen-1] == 'i' &&
	    de->d_name[dlen-2] == '.')
	    continue;
	if (do_match && !match_string(regexp, de->d_name))
	    continue;
	if ((size_t)(dlen + pos) >= sizeof name_buffer) {
	    truncated = 1;
	    break;
	}
	if (dlen > max)
	    max = dlen;
	pos += dlen + 1;
	num++;
    }
    if (num == 0) {
	closedir(dirp);
	return;
    }
    max++;
    if (max > (int)sizeof buff - 1)
	max = (int)sizeof buff - 1;
    /*
     * Copy all file names into a name space, and set up pointer
     * into it.
     */
    names = (char **)xalloc(num * sizeof (char *));
    rewinddir(dirp);
    for(pos=0, i=0, de = readdir(dirp); i < num; de = readdir(dirp)) {
	int dlen = strlen(de->d_name);
	if (!do_match && (strcmp(de->d_name, ".") == 0 ||
			  strcmp(de->d_name, "..") == 0))
	    continue;
	if (!do_match && dlen >= 2 &&
	    de->d_name[dlen-1] == 'i' &&
	    de->d_name[dlen-2] == '.')
	    continue;
	if (do_match && !match_string(regexp, de->d_name))
	    continue;
	names[i] = name_buffer + pos;
	strcpy(name_buffer + pos, de->d_name);
	pos += dlen + 1;
	i++;
    }
    /* Sort the names. */
    qsort((char *)names, num, sizeof (char *), pstrcmp);
    /*
     * Print the file names in nice columns, as many as possible
     * on every line. That means that there can be one extra name
     * on each line, depending on the size of the last name on the line.
     */
    buff[0] = '\0';
    for(lines=0, i=0, j=0; j<num; j++) {
	if (i * max + strlen(names[j]) >= 79) {
	    /*
	     * Strip the last spaces on the last name to ensure that
	     * it will fit last on the line.
	     */
	    if (i > 0) {
		for (i=strlen(buff) - 1; i >= 0 && buff[i] == ' '; i--)
		    buff[i] = '\0';
		add_message("%s\n", buff);
	    }
	    i = 0;
	    lines++;
	    if (lines > MAX_LINES) {
		truncated = 1;
		break;
	    }
	}
	{
	    int off = i * max;
	    int rem = sizeof(buff) - off;
	    if (rem <= 1) {
		truncated = 1;
		break;
	    }
	    (void)snprintf(buff + off, (size_t)rem, "%-*.*s",
			   max, max - 1, names[j]);
	}
	i++;
    }
    if (i)
	add_message("%s\n", buff);
    if (truncated)
	add_message("***TRUNCATED***\n");
    free((char *)names);
    closedir(dirp);
}

int tail(char *path)
{
    char buff[1000];
    FILE *f;
    struct stat st;
    int offset;

    path = check_file_name(path, 0);
    if (path == 0)
        return 0;
    f = fopen(path, "r");
    if (f == 0)
	return 0;
    if (fstat(fileno(f), &st) == -1)
	fatal("Could not stat an open file.\n");
    offset = st.st_size - 54 * 20;
    if (offset < 0)
	offset = 0;
    if (fseek(f, offset, 0) == -1)
	fatal("Could not seek.\n");
    /* Throw away the first incomplete line. */
    if (offset > 0 && !fgets(buff, sizeof buff, f) && ferror(f)) {
	fclose(f);
	return 0;
    }
    if (feof(f)) {
	fclose(f);
	return 1;
    }
    while(fgets(buff, sizeof buff, f)) {
	add_message("%s", buff);
    }
    if (ferror(f)) {
	fclose(f);
	return 0;
    }
    fclose(f);
    return 1;
}

struct value *print_file(char *path, int start, int len)
{
    char buff[1000];
    FILE *f;
    int i;

    if (len < 0)
	return &const0;
    path = check_file_name(path, 0);
    if (path == 0)
        return &const0;
    if (start < 0)
	return &const0;
    f = fopen(path, "r");
    if (f == 0)
	return &const0;
    if (len == 0)
	len = MAX_LINES;
    if (len > MAX_LINES)
	len = MAX_LINES;
    if (start == 0)
	start = 1;
    for (i=1; i < start + len; i++) {
	if (fgets(buff, sizeof buff, f) == 0)
	    break;
	if (i >= start)
	    add_message("%s", buff);
    }
    fclose(f);
    if (i <= start)
	return &const0;
    if (i == MAX_LINES + start)
	add_message("*****TRUNCATED****\n");
    return &const1;
}

void remove_file(char *path)
{
    path = check_file_name(path, 1);
    if (path == 0)
        return;
    if (unlink(path) == -1)
        add_message("No such file: %s\n", path);
    return;
}

void people() {
    struct object *ob;
    struct value *player;

    for (ob = obj_list; ob; ob=ob->next_all) {
	if (!ob->interactive)
	    continue;
	player = apply("query_name", ob, 0);
	if (player == 0 || player->type != T_STRING)
	    continue;
	if (!ob->super)
	    continue;
	show_info_about(player->u.string, ob->super->name, ob->interactive);
    }
}

void log_file(char *file, char *str)
{
    FILE *f;
    char file_name[100];

    if (strchr(file, '/') || file[0] == '.' || strlen(file) > 30)
	error("Illegal file name to log_file(%s)\n", file);
    sprintf(file_name, "log/%s", file);
    f = fopen(file_name, "a");
    if (f == 0)
	return;
    fwrite(str, strlen(str), 1, f);
    fclose(f);
}

struct value *call_indirect_impl(int fun, intptr_t arg, intptr_t arg2,
				 intptr_t arg3)
{
    struct value *ret;
    struct object *o1, *o2;

    switch(fun) {
    case F_FIND_OBJECT:
	o1 = find_object2((char *)(uintptr_t)arg);
	if (!o1)
	    return &const0;
	ret = alloc_value();
	ret->type = T_OBJECT;
	ret->u.ob = o1;
	add_ref(o1, "F_FIND_OBJECT");
	return ret;
    case F_SNOOP:
	if (!has_efun_privilege(20, "snoop"))
	    return 0;
	if (arg && ((struct object *)(uintptr_t)arg)->destructed)
	     error("snoop() on destructed object.\n");
	set_snoop(current_object, (struct object *)(uintptr_t)arg);
	return 0;
    case F_SET_HEART_BEAT:
	if (set_heart_beat(current_object, (int)arg))
	    return &const1;
	else
	    return &const0;
    case F_LOG_FILE:
	log_file((char *)(uintptr_t)arg, (char *)(uintptr_t)arg2);
	return 0;
    case F_SHUTDOWN:
	if (strcmp(current_object->name, "obj/shut") != 0 &&
	    !has_efun_privilege(20, "shutdown"))
	    return 0;
	startshutdowngame(0);
	return 0;
    case F_LIVING:
	if (((struct object *)(uintptr_t)arg)->destructed)
	    return &const0;
	ret = alloc_value();
	ret->type = T_NUMBER;
	ret->u.number = ((struct object *)(uintptr_t)arg)->enable_commands;
	return ret;
    case F_ED:
    {
	char *path = 0;
	if (!has_efun_privilege(21, "ed"))
	    return 0;
	if (arg) {
	    path = check_file_name((char *)(uintptr_t)arg, 1);
	    if (!path)
		return 0;
	    if (!legal_path(path)) {
		add_message("Illegal path\n");
		return 0;
	    }
	}
	ed_start(path);
	return 0;
    }
    case F_TELL_OBJECT:
	if (((struct object *)(uintptr_t)arg)->destructed)
	    error("Tell_object to destructed object./\n");
	tell_object((struct object *)(uintptr_t)arg, (char *)(uintptr_t)arg2);
	return 0;
    case F_RM:
	remove_file((char *)(uintptr_t)arg);
	return 0;
    case F_LS:
	list_files((char *)(uintptr_t)arg, (char *)0);
	return 0;
    case F_CAT:
	return print_file((char *)(uintptr_t)arg, (int)arg2, (int)arg3);
    case F_INPUT_TO:
	if (current_object->destructed)
	     error("input_to() on destructed object.\n");
	return input_to((char *)(uintptr_t)arg, (int)arg2);
	break;
    case F_ENABLE_COMMANDS:
	enable_commands(1);
	return 0;
    case F_FIRST_INVENTORY:
	return first_inventory((struct value *)(uintptr_t)arg);
    case F_NEXT_INVENTORY:
	return next_inventory((struct value *)(uintptr_t)arg);
    case F_SHOUT:
	shout_string((char *)(uintptr_t)arg);
	return 0;
    case F_SAY:
	say((struct value *)(uintptr_t)arg, (struct object *)(uintptr_t)arg2);
	return 0;
    case F_TELL_ROOM:
	if (((struct object *)(uintptr_t)arg)->destructed)
	    error("tell_room to destructed object.\n");
	tell_room((struct object *)(uintptr_t)arg, (struct value *)(uintptr_t)arg2);
	return 0;
    case F_CREATE_WIZARD:
	if (strcmp(current_object->name, "room/port_castle") != 0)
	    error("Illegal use of create_wizard()\n");
	return create_wizard((char *)(uintptr_t)arg);
    case F_DESTRUCT:
	destruct_object((struct value *)(uintptr_t)arg);
	return 0;
    case F_SET_LIGHT:
	add_light(current_object, (int)arg);
	ret = alloc_value();
	ret->type = T_NUMBER;
	o1 = current_object;
	while(o1->super)
	    o1 = o1->super;
	ret->u.number = o1->total_light;
	return ret;
    case F_COMMAND:
	if (command_for_object((char *)(uintptr_t)arg,
			       (struct object *)(uintptr_t)arg2))
	    return &const1;
	else
	    return &const0;
    case F_PRESENT:
	if (arg2 && ((struct object *)(uintptr_t)arg2)->destructed)
	    error("present() on destructed object.\n");
	return object_present((struct value *)(uintptr_t)arg,
			      (struct object *)(uintptr_t)arg2);
    case F_ENVIRONMENT:
	return environment((struct value *)(uintptr_t)arg);
    case F_SAVE_OBJECT:
	save_object(current_object, (char *)(uintptr_t)arg);
	return 0;
    case F_RESTORE_OBJECT:
	ret = alloc_value();
	ret->type = T_NUMBER;
	ret->u.number = restore_object(current_object, (char *)(uintptr_t)arg);
	return ret;
    case F_CLONE_OBJECT:
	return clone_object((char *)(uintptr_t)arg);
    case F_FUNCTION:
	return call_local_function((char *)(uintptr_t)arg,
				   (struct value *)(uintptr_t)arg2);
    case F_CALL_OTHER:
	return call_other((struct value *)(uintptr_t)arg, (char *)(uintptr_t)arg2,
			  (struct value *)(uintptr_t)arg3);
    case F_WRITE:
	do_write((struct value *)(uintptr_t)arg);
	break;
    case F_MOVE_OBJECT:
	if (((struct value *)(uintptr_t)arg)->type == T_OBJECT)
	    o1 = ((struct value *)(uintptr_t)arg)->u.ob;
	else {
	    o1 = find_object(((struct value *)(uintptr_t)arg)->u.string);
	    if (o1 == 0)
		error("Object %s not found.\n",
		      ((struct value *)(uintptr_t)arg)->u.string);
	}
	if (((struct value *)(uintptr_t)arg2)->type == T_OBJECT)
	    o2 = ((struct value *)(uintptr_t)arg2)->u.ob;
	else {
	    o2 = find_object(((struct value *)(uintptr_t)arg2)->u.string);
	    if (o2 == 0)
		error("Object %s not found.\n",
		      ((struct value *)(uintptr_t)arg2)->u.string);
	}
	if (((struct object *)o1)->destructed)
	     error("move_object() of destructed object.\n");
	if (((struct object *)o2)->destructed)
	     error("move_object() to destructed object.\n");
	move_object(o1, o2);
	break;
    case F_ADD_VERB:
	add_verb((char *)(uintptr_t)arg);
	break;
    case F_THIS_PLAYER:
	return this_player();
    case F_THIS_OBJECT:
	return this_object();
    default:
	fatal("Unimplemented hard linked function %d\n", fun);
	abort();
    }
    return 0;
}

void do_write(struct value *arg)
{
    if (arg == 0)
	add_message("<NULL>");
    else if (arg->type == T_STRING) {
	if (strlen(arg->u.string) > 9500)	/* Not pretty */
	    error("Too long string.\n");
	add_message("%s", arg->u.string);
    } else if (arg->type == T_OBJECT)
	add_message("OBJ(%s)", arg->u.ob->name);
    else if (arg->type == T_NUMBER)
	add_message("%d", arg->u.number);
    else
	add_message("<UNKNOWN>");
}

/* Find an object. If not loaded, load it ! */

struct object *find_object(char *str)
{
    struct object *ob;

    /* Remove leading '/' if any. */
    while(str[0] == '/')
	str++;
    ob = find_object2(str);
    if (ob)
	return ob;
    ob = load_object(str);
    if (ob->destructed)		/* *sigh* */
	return 0;
    if (ob->swapped)
	load_ob_from_swap(ob);
    return ob;
}

/* Look for a loaded object. */
struct object *find_object2(char *str)
{
    register struct object *ob;
    size_t length;
    char *name;

    /* Remove leading '/' if any. */
    while(str[0] == '/')
	str++;
    /* Truncate possible .c in the object name. */
    length = strlen(str);
    name = string_copy(str);
    if (length >= 2 && name[length-2] == '.' && name[length-1] == 'c') {
	name[length-2] = '\0';
    }
    if ((ob = lookup_object_hash(name))) {
	free(name);
	if (ob->swapped)
	    load_ob_from_swap(ob);
	return ob;
    }
    free(name);
    return 0;
}

void apply_command(char *com)
{
    struct value *ret;

    if (command_giver == 0)
	error("command_giver == 0 !\n");
    ret = apply(com, command_giver->super, 0);
    if (ret != 0) {
	add_message("Result:");
	if (ret->type == T_STRING)
	    add_message("%s\n", ret->u.string);
	if (ret->type == T_NUMBER)
	    add_message("%d\n", ret->u.number);
    } else {
	add_message("Error apply_command: function %s not found.\n", com);
    }
}

#if 0
/* This is merged into move_object() */
void set_current_room(ob, dest)
    struct object *ob, *dest;
{
    struct object **pp, *p;
    struct object *save_command_giver = command_giver;

    if (dest == 0)
	dest = find_object("room/void.c");	/* Get any existing void. */
    if (dest == 0)
	fatal("Not even a void !\n");
    command_giver = ob;
    if (ob->super) {
	struct value v;

	v.type = T_OBJECT;
	v.u.ob = ob;		/* No need to increment ref count */
	(void)apply("exit", ob->super, &v);
	add_light(ob->super, - ob->total_light);
	remove_sent(ob->super, ob);
	/*
	 * Now we link the ob out of its list.
	 * Remove sentences tied to objects that stays in this room.
	 */
	for (pp = &ob->super->contains; *pp;) {
	    if (*pp == ob)
		*pp = (*pp)->next_inv;
	    else {
		remove_sent(*pp, ob);
		pp = &(*pp)->next_inv;
	    }
	}
    }
    ob->next_inv = dest->contains;
    dest->contains = ob;
    add_light(dest, ob->total_light);
    ob->super = dest;
    if (d_flag)
	debug_message("--Current room: %s\n", dest->name);
    (void)apply("init", dest, 0);
    for (p = dest->contains; p; p = p->next_inv) {
	if (p == ob)
	    continue;
	(void)apply("init", p, 0);
    }
    command_giver = save_command_giver;
}
#endif

/*
 * Transfer an object.
 * The object has to be taken from one inventory list and added to another.
 * The main work is to update all command definitions, depending on what is
 * living or not. Note that all objects in the same inventory are affected.
 */
void move_object(struct object *item, struct object *dest)
{
    struct object **pp, *ob;
    struct object *save_cmd = command_giver;

    /* Recursive moves are not allowed. */
    for (ob = dest; ob; ob = ob->super)
	if (ob == item)
	    return;
    if (item->not_touched) {
	item->not_touched = 0;
	(void)apply("reset", item, &const1);
	if (item->destructed)
	    return;
    }
    if (dest->not_touched) {
	dest->not_touched = 0;
	(void)apply("reset", dest, &const1);
	if (dest->destructed)
	    return;
    }
    if (item->super) {
	int okey = 0;
		
	if (item->enable_commands) {
	    struct value v;

	    v.type = T_OBJECT;
	    v.u.ob = item;		/* No need to increment ref count */
	    command_giver = item;
	    (void)apply("exit", item->super, &v);
	    remove_sent(item->super, item);
	}
	if (item->super->enable_commands)
	    remove_sent(item, item->super);
	add_light(item->super, - item->total_light);
	for (pp = &item->super->contains; *pp;) {
	    if (*pp != item) {
		if ((*pp)->enable_commands)
		    remove_sent(item, *pp);
		if (item->enable_commands)
		    remove_sent(*pp, item);
		pp = &(*pp)->next_inv;
		continue;
	    }
	    *pp = item->next_inv;
	    okey = 1;
	}
	if (!okey)
	    fatal("Failed to find object %s in super list of %s.\n",
		  item->name, item->super->name);
    }
    item->next_inv = dest->contains;
    dest->contains = item;
    item->super = dest;
    /*
     * Setup the new commands. The order is very important, as commands
     * in the room should override commands defined by the room.
     */
    if (item->enable_commands) {
	command_giver = item;
	(void)apply("init", dest, 0);
	if (dest->destructed)
	    return;
    }
    /*
     * Run init of the item once for every present player, and
     * for the environment (which can be a player).
     */
    for (ob = dest->contains; ob; ob=ob->next_inv) {
	if (ob == item)
	    continue;
	if (ob->enable_commands) {
	    command_giver = ob;
	    (void)apply("init", item, 0);
	}
	if (item->enable_commands) {
	    command_giver = item;
	    (void)apply("init", ob, 0);
	}
    }
    if (dest->enable_commands) {
	command_giver = dest;
	(void)apply("init", item, 0);
    }
    command_giver = save_cmd;
    add_light(dest, item->total_light);
    if (d_flag)
	debug_message("--move_object: %s to %s\n", item->name, dest->name);
}

/*
 * Every object as a count of number of light sources it contains.
 * Update this.
 */

void add_light(struct object *p, int n)
{
    if (n == 0)
	return;
    p->total_light += n;
    if (p->super)
	add_light(p->super, n);
}

struct sentence *sent_free = 0;
int tot_alloc_sentence;

struct sentence *alloc_sentence() {
    struct sentence *p;

    if (sent_free == 0) {
	p = (struct sentence *)xalloc(sizeof *p);
	tot_alloc_sentence++;
    } else {
	p = sent_free;
	sent_free = sent_free->next;
    }
    p->verb = 0;
    p->function = 0;
    p->next = 0;
    return p;
}

#ifdef free
void free_all_sent() {
    struct sentence *p;
    for (;sent_free; sent_free = p) {
	p = sent_free->next;
	free(sent_free);
    }
}
#endif

void free_sentence(struct sentence *p)
{
    if (p->function)
	free_string(p->function);
    p->function = 0;
    if (p->verb)
	free_string(p->verb);
    p->verb = 0;
    p->next = sent_free;
    sent_free = p;
}

/*
 * Find the sentence for a command from the player.
 * Return success status.
 */
int player_parser(char *buff)
{
    struct sentence *s;
    char *p;
    int length;
    struct object *save_current_object = current_object;
    char verb_copy[20];

    if (d_flag)
	debug_message("cmd [%s]: %s\n", command_giver->name, buff);
    /* strip trailing spaces. */
    for (p = buff + strlen(buff) - 1; p > buff; p--) {
	if (*p != ' ')
	    break;
	*p = '\0';
    }
    if (buff[0] == '\0')
	return 0;
    if (special_parse(buff))
	return 1;
    p = strchr(buff, ' ');
    if (p == 0)
	length = strlen(buff);
    else
	length = p - buff;
    clear_notify();
	for (s=command_giver->sent; s; s = s->next) {
	    struct value *ret;
	    struct value arg;
	    const char *called_function;
	    int len;
		
	    if (s->verb == 0)
		continue;
	len = strlen(s->verb);
	if (s->short_verb) {
	    if (strncmp(s->verb, buff, len) != 0)
		continue;
	} else {
		    if (strlen(s->verb) != (size_t)length)
		continue;
	    if (strncmp(buff, s->verb, length) != 0)
		continue;
	}
	/*
	 * Now we have found a special sentence !
	 */
	if (d_flag)
	    debug_message("Local command %s on %s\n", s->function, s->ob->name);
		if ((size_t)length >= sizeof verb_copy)
		    len = sizeof verb_copy - 1;
		else
		    len = length;
	strncpy(verb_copy, buff, len);
	verb_copy[len] = '\0';
	last_verb = verb_copy;
	/*
	 * If the function is static and not defined by current object,
	 * then it will fail. If this is called directly from player input,
	 * then we set current_object so that static functions are allowed.
	 */
	    if (current_object == 0)
		current_object = s->ob;
	    called_function = s->function;
	    if (buff[length] == ' ') {
		arg.type = T_STRING;
		arg.u.string = &buff[length+1];
		ret = apply(s->function, s->ob, &arg);
	} else {
	    ret = apply(s->function, s->ob, 0);
	}
	if (current_object->destructed)
	    s = command_giver->sent;	/* Restart :-( */
	last_verb = 0;
	current_object = save_current_object;
	/* If we get fail from the call, it was wrong second argument. */
	if (ret && ret->type == T_NUMBER && ret->u.number == 0)
	    continue;
	    if (s && s->ob->wl && command_giver->interactive &&
	      !command_giver->is_wizard)
		s->ob->wl->score++;
	    if (ret == 0)
		add_message("Error: function %s not found.\n",
			    called_function ? called_function : "<null>");
	    break;
	}
    if (s == 0) {
	notify_no_command();
	return 0;
    }
    return 1;
}

/*
 * Associate a command with function in this object.
 * The optional second argument is the command name. If the command name
 * is not given here, it should be given with add_verb().
 *
 * The optinal third argument is a flag that will state that the verb should
 * only match against leading characters.
 *
 * The object must be near the command giver, so that we ensure that the
 * sentence is removed when the command giver leaves.
 */
void add_action(char *str, char *cmd, int flag)
{
    struct sentence *p;

    if (command_giver == 0)
	return;
    if (current_object != command_giver &&
	current_object->super != command_giver &&
	current_object->super != command_giver->super &&
	current_object != command_giver->super)
      error("add_action from object that was not present.\n");
    if (d_flag)
	debug_message("--Add action %s\n", str);
    if (strcmp(str, "exit") == 0)
	error("Illegal to define a command to the exit() function.\n");
    p = alloc_sentence();
    p->function = make_shared_string(str);
    p->ob = current_object;
    p->next = command_giver->sent;
    p->short_verb = flag;
    if (cmd)
	p->verb = make_shared_string(cmd);
    else
	p->verb = 0;
    command_giver->sent = p;
}

void add_verb(char *str)
{
    if (command_giver == 0)
	return;
    if (command_giver->sent == 0)
	error("No add_action().\n");
    if (command_giver->sent->verb != 0)
	error("Tried to set verb again.\n");
    command_giver->sent->verb = make_shared_string(str);
    if (d_flag)
	debug_message("--Adding verb %s to action %s\n", str,
		command_giver->sent->function);
}

void remove_sent(struct object *ob, struct object *player)
{
    struct sentence **s;

    for (s= &player->sent; *s;) {
	struct sentence *tmp;
	if ((*s)->ob == ob) {
	    if (d_flag)
		debug_message("--Unlinking sentence %s\n", (*s)->function);
	    tmp = *s;
	    *s = tmp->next;
	    free_sentence(tmp);
	} else
	    s = &((*s)->next);
    }
}

#if 0
/* This function has been replaced by using users(). */
void display_all_players() {
    struct object *ob;
    struct value *ret;

    for (ob = obj_list; ob; ob = ob->next_all) {
	if (ob->interactive == 0)
	    continue;
	ret = apply("short", ob, 0);
	if (ret && ret->type == T_STRING)
	    add_message("%s.\n", ret->u.string);
    }
}
#endif

char debug_parse_buff[50]; /* Used for debugging */

int special_parse(char *buff)
{
/*
    if (strcmp(buff, "who") == 0) {
	display_all_players();
	return 1;
    }
*/
    strncpy(debug_parse_buff, buff, sizeof debug_parse_buff);
    if (strcmp(buff, "malloc") == 0) {
#if defined(MALLOC_malloc) || defined(MALLOC_smalloc)
	dump_malloc_data();
#endif
#ifdef MALLOC_gmalloc
	add_message("Using Gnu malloc.\n");
#endif
	return 1;
    }
    if (strcmp(buff, "dumpallobj") == 0) {
        dumpstat();
	return 1;
    }
#if defined(MALLOC_malloc) || defined(MALLOC_smalloc)
    if (strcmp(buff, "debugmalloc") == 0) {
	extern int debugmalloc;
	debugmalloc = !debugmalloc;
	if (debugmalloc)
	    add_message("On.\n");
	else
	    add_message("Off.\n");
	return 1;
    }
#endif
    if (strcmp(buff, "status") == 0) {
	extern int tot_alloc_sentence, tot_alloc_value, tot_alloc_object,
		   num_swapped, total_bytes_swapped,
		   tot_string_space;
	extern int total_num_prog_blocks, total_prog_block_size;
	add_message("Sentences:   %5d %6d\n", tot_alloc_sentence,
	       tot_alloc_sentence * sizeof (struct sentence));
	add_message("Objects:     %5d %6d (%d swapped, %d Kbyte)\n",
		    tot_alloc_object,
		    tot_alloc_object * sizeof (struct object), num_swapped,
		    total_bytes_swapped / 1024);
	add_message("Prog blocks: %5d %6d\n", total_num_prog_blocks,
		    total_prog_block_size);
	add_message("Values:      %5d %6d\n\n", tot_alloc_value,
	       tot_alloc_value * sizeof (struct value));
	/* status_lnodes_matched(); */
	stat_living_objects();
	print_lnode_status(tot_alloc_sentence * sizeof (struct sentence) +
			   tot_alloc_object * sizeof (struct object) +
			   tot_alloc_value * sizeof (struct value) +
			   total_prog_block_size +
			   show_otable_status() +
			   heart_beat_status() +
			   add_string_status() +
			   print_call_out_usage());
	return 1;
    }
    if (strcmp(buff, "e") == 0) {
	(void)strcpy(buff, "east");
	return 0;
    }
    if (strcmp(buff, "w") == 0) {
	(void)strcpy(buff, "west");
	return 0;
    }
    if (strcmp(buff, "s") == 0) {
	(void)strcpy(buff, "south");
	return 0;
    }
    if (strcmp(buff, "n") == 0) {
	(void)strcpy(buff, "north");
	return 0;
    }
    if (strcmp(buff, "ne") == 0) {
	(void)strcpy(buff, "northeast");
	return 0;
    }
    if (strcmp(buff, "nw") == 0) {
	(void)strcpy(buff, "northwest");
	return 0;
    }
    if (strcmp(buff, "se") == 0) {
	(void)strcpy(buff, "southeast");
	return 0;
    }
    if (strcmp(buff, "sw") == 0) {
	(void)strcpy(buff, "southwest");
	return 0;
    }
    if (strcmp(buff, "d") == 0) {
	(void)strcpy(buff, "down");
	return 0;
    }
    if (strcmp(buff, "u") == 0) {
	(void)strcpy(buff, "up");
	return 0;
    }
    if (strcmp(buff, "nw") == 0) {
	(void)strcpy(buff, "northwest");
	return 0;
    }
    if (strcmp(buff, "ne") == 0) {
	(void)strcpy(buff, "northeast");
	return 0;
    }
    if (strcmp(buff, "sw") == 0) {
	(void)strcpy(buff, "southwest");
	return 0;
    }
    if (strcmp(buff, "se") == 0) {
	(void)strcpy(buff, "southeast");
	return 0;
    }
    return 0;
}

void print_local_commands() {
    struct sentence *s;

    add_message("Current local commands:\n");
    for (s = command_giver->sent; s; s = s->next)
	add_message("%s ", s->verb);
    add_message("\n");
}

/*VARARGS1*/
void fatal(char *fmt, ...)
{
    va_list ap;
    char msg[2000];

    static int in_fatal = 0;
    /* Prevent double fatal. */
    if (in_fatal)
	abort();
    in_fatal = 1;
    va_start(ap, fmt);
    (void)vsnprintf(msg, sizeof msg, fmt, ap);
    va_end(ap);
    (void)fprintf(stderr, "%s", msg);
    fflush(stderr);
    if (current_object)
	(void)fprintf(stderr, "Current object was %s\n",
		      current_object->name);
    debug_message("%s", msg);
    if (current_object)
	debug_message("Current object was %s\n", current_object->name);
    debug_message("Dump of variables:\n");
#ifdef TRACE
    (void)dump_trace();
#endif
    abort();
}

int num_error = 0;

/*
 * Error() has been "fixed" so that users can catch and throw them.
 * To catch them nicely, we really have to provide decent error information.
 * Hence, all errors that are to be caught
 * (error_recovery_context_exists == 2) construct a string containing
 * the error message, which is returned as the
 * thrown value.  Users can throw their own error values however they choose.
 */

/*
 * This is here because throw constructs its own return value; we dont
 * want to replace it with the system's error string.
 */

void throw_error() {
    extern int error_recovery_context_exists;
    extern jmp_buf error_recovery_context;
    if (error_recovery_context_exists > 1) {
	longjmp(error_recovery_context, 1);
	fatal("Throw_error failed!");
    }
    error("Throw with no catch.\n");
}

static char emsg_buf[2000];

/*VARARGS1*/
void error(char *fmt, ...)
{
    va_list ap;
    char *object_name;
    extern int error_recovery_context_exists;
    extern jmp_buf error_recovery_context;
    extern struct object *current_heart_beat;
    extern struct value * catch_value;
#ifdef TRACE
    extern int trace_depth;
#endif

    va_start(ap, fmt);
    vsnprintf(emsg_buf + 1, sizeof(emsg_buf) - 1, fmt, ap);
    va_end(ap);
    emsg_buf[0] = '*';	/* all system errors get a * at the start */
    if (error_recovery_context_exists > 1) { /* user catches this error */
   	catch_value = make_string(emsg_buf);
   	longjmp(error_recovery_context, 1);
   	fatal("Catch() longjump failed");
    }
    num_error++;
    if (num_error > 1)
	fatal("Too many simultaneous errors.\n");
    debug_message("%s", emsg_buf + 1);
    if (current_object) {
	debug_message("Current object was %s, line %d\n",
		      current_object->name, current_line);
	save_error(emsg_buf, current_object->name, current_line);
    }
#ifdef TRACE
    object_name = dump_trace();
    fflush(stdout);
    if (object_name) {
	struct object *ob;
	ob = find_object2(object_name);
	if (!ob) {
	    if (command_giver)
		add_message("error when executing program in destroyed object %s\n",
			    object_name);
	    debug_message("error when executing program in destroyed object %s\n",
			object_name);
	}
    }
    trace_depth = 0;
#endif
    if (command_giver) {
	add_message("%s", emsg_buf + 1);
	if (current_object)
	    add_message("Current object was %s, line %d\n",
			current_object->name, current_line);
#if 0
	if (!command_giver->destructed) {
	    dest = find_object("room/void.c");	/* Get any existing void. */
	    if (dest == 0)
		fatal("Could not find the void room.\n");
	    move_object(command_giver, dest);
	}
#endif
    }
    if (current_heart_beat) {
	set_heart_beat(current_heart_beat, 0);
	debug_message("Heart beat in %s turned off.\n",
		      current_heart_beat->name);
	if (current_heart_beat->interactive) {
	    struct object *save_cmd = command_giver;
	    command_giver = current_heart_beat;
	    add_message("Game driver tells you: You have no heart beat !\n");
	    command_giver = save_cmd;
	}
	current_heart_beat = 0;
    }
#if 0
    debug_message("Dump of variables:\n");
    if (current_object) {
	struct lnode_var_def *p;
	for (p=current_object->status; p; p = p->next) {
	    debug_message("%20s: ", p->name);
	    debug_message_value(&current_object->variables[p->num_var]);
	    debug_message("\n");
	}
    }
#endif
    num_error--;
    if (error_recovery_context_exists)
	longjmp(error_recovery_context, 1);
    abort();
}

#ifdef TRACE
char *get_current_object_name() {
    if (current_object == 0)
	return "NONE";
    return current_object->name;
}

char *get_command_giver_name() {
    if (command_giver == 0)
	return "NONE";
    return command_giver->name;
}
#endif

void pre_compile(char *str)
{
    char *i_name;
    int pid;
    int fd, fd2;
    unsigned int save_alarm;

    i_name = xalloc(strlen(str)+3);
    (void)strcpy(i_name, str);
    (void)strcat(i_name, ".i");
    /*
     * We don't want the child process to have an alarm signal.
     * The alarm might as well be removed here, instead of in the child
     * after vfork(), because some Unix (read: apollo) will remove the alarm
     * for the parent also when done in vfork before exec().
     */
    save_alarm = alarm(0);
#ifdef USE_VFORK
    pid = vfork();
#else
    pid = fork();
#endif
    if (pid < 0) {
	(void)alarm(save_alarm);
	free(i_name);
	perror("fork");
	error("Could not fork precompiler process.\n");
    }
    if (pid > 0) {
	/* This is the parent. Wait for the childe to complete. */
	int result;

	/* Enable the alarm again. */
	(void)alarm(save_alarm);
	while(1) {
	    if (wait(&result) != -1)
		break;
	    if (errno == EINTR)
		continue;
	    perror("wait");
	    abort();
	}
#ifdef USE_VFORK
	/*
	 * When using vfork(), i_name can have been changed by the child
	 * into name.c. Change this back.
	 */
	i_name[strlen(i_name) - 1] = 'i';
#endif
	/*
	 * Remove the i-file here, because the file might contain include
	 * of files from ../../../ etc. The i-file will also be removed
	 * if there is a compilation error.
	 */
	if (result != 0)
	    unlink(i_name);
	free(i_name);
	if (result == 0)
	    return;
	error("Precompiler returned signal %d, return code %d\n",
	      result & 0xf, result >> 8);
    }
    fd = creat(i_name, 0666);
    if (fd < 0) {
	perror(i_name);
	/*
	 * Do not call exit() now, which would close all files for parent.
	 */
	_exit(1);
    }
    else if ((fd2 = dup2(fd, 1)) != 1) {
	fprintf(stderr, "FATAL: failed to dup2. fd2 = %d, errno = %d, fd = %d\n",
		fd2, errno, fd);
	_exit(1);
    }
    close(fd);
    (void)strcpy(i_name, str);
    (void)strcat(i_name, ".c");
    if (PRE_COMPILE(i_name) == -1) {
	perror("precompile");
	_exit(99);
    }
    _exit(0);
}

/*
 * Check that it is an legal path. No '..' are allowed.
 */
int legal_path(char *path)
{
    char *p;

    if (path == NULL || strchr(path, ' '))
	return 0;
    if (path[0] == '/')
        return 0;
    for(p = strchr(path, '.'); p; p = strchr(p+1, '.')) {
	if (p[1] == '.')
	    return 0;
    }
    return 1;
}

void smart_log(char *error_file, int line, char *what)
{
    char buff2[100], buff[100], *p;
    int n;

    if (error_file == 0)
	return;
    n = sscanf(error_file, "players/%99s", buff2);
    if (n != 1)
        return;
    p = strchr(buff2, '/');
    if (p)
        *p = '\0';
    sprintf(buff, "%s line %d:%s\n", error_file, line, what);
    log_file(buff2, buff);
}

/*
 * Check that a file name is valid for read or write.
 * Also change the name as if the current directory was at the players
 * own directory.
 * This is done by functions in the player object.
 */
char debug_check_file[50];

char *check_file_name(char *file, int writeflg)
{
    struct value v, *ret;

    if (!command_giver)
	return 0;
    v.type = T_STRING;
    v.u.string = file;
    /*
     * We don't have to free the string in ret. This is done
     * by the garbage collection.
     */
    if (writeflg)
	ret = apply("valid_write", command_giver, &v);
    else
	ret = apply("valid_read",  command_giver, &v);
    if (command_giver->destructed)
	return 0;
    if (ret->type != T_STRING) {
	add_message("Bad file name.\n");
	return 0;
    }
    strncpy(debug_check_file, ret->u.string, sizeof debug_check_file);
    if (!legal_path(ret->u.string)) {
        add_message("Illegal path\n");
	return 0;
    }
    return ret->u.string;
}

/*
 * This one is called from HUP.
 */
int game_is_being_shut_down;

void startshutdowngame(int sig) {
    (void)sig;
    game_is_being_shut_down = 1;
}

/*
 * This one is called from the command "shutdown".
 * We don't call it directly from HUP, because it is dangerous when being
 * in an interrupt.
 */
void shutdowngame() {
    shout_string("Game driver shouts: LPmud shutting down immediately.\n");
    save_wiz_file();
    ipc_remove();
    remove_all_players();
#ifdef free
    remove_all_objects();
    free_all_sent();
    remove_wiz_list();
    dump_malloc_data();
    find_alloced_data();
#endif
    exit(0);
}

/*
 * Transfer an object from an object to an object.
 * Call add_weight(), drop(), get(), prevent_insert(), add_weight(),
 * and can_put_and_get() where needed.
 * Return 0 on success, and special code on failure:
 *
 * 1: To heavy for destination.
 * 2: Can't be dropped.
 * 3: Can't take it out of it's container.
 * 4: The object can't be inserted into bags etc.
 * 5: The destination doesn't allow insertions of objects.
 * 6: The object can't be picked up.
 */
int transfer_object(struct object *ob, struct object *to)
{
    struct value *weight, neg_weight, *ret;
    struct object *from = ob->super;

    /*
     * Get the weight of the object
     */
    weight = apply("query_weight", ob, 0);
    if (weight && weight->type != T_NUMBER)
	error("Bad type the weight of object in transfer()\n");
    if (ob->destructed)
	return 3;
    /*
     * If the original place of the object is a living object,
     * then we must call drop() to check that the object can be dropped.
     */
    if (from && from->enable_commands) {
	ret = apply("drop", ob, 0);
	if (ret && (ret->type != T_NUMBER || ret->u.number != 0))
	    return 2;
	/* This shold not happen, but we can't trust LPC hackers. :-) */
	if (ob->destructed)
	    return 2;
    }
    /*
     * If 'from' is not a room and not a player, check that we may
     * remove things out of it.
     */
    if (from && from->super && !from->enable_commands) {
	ret = apply("can_put_and_get", from, 0);
	if (!ret || ret->type != T_NUMBER || ret->u.number == 0 ||
	  from->destructed)
	    return 3;
    }
    /*
     * If the destination is not a room, and not a player,
     * Then we must test 'prevent_insert', and 'can_put_and_get'.
     */
    if (to->super && to->enable_commands == 0) {
	ret = apply("prevent_insert", ob, 0);
	if (ret && (ret->type != T_NUMBER || ret->u.number != 0))
	    return 4;
	ret = apply("can_put_and_get", to, 0);
	if (!ret || ret->type != T_NUMBER || ret->u.number == 0 ||
	  to->destructed || ob->destructed)
	    return 5;
    }
    /*
     * If the destination is a player, check that he can pick it up.
     */
    if (to->enable_commands) {
	ret = apply("get", ob, 0);
	if (!ret || (ret->type == T_NUMBER && ret->u.number == 0) ||
	  ob->destructed)
	    return 6;
    }
    /*
     * If it is not a room, correct the total weight in the destination.
     */
    if (to->super && weight) {
	/*
	 * Check if the destination can carry that much.
	 */
	ret = apply("add_weight", to, weight);
	if (ret && ret->type == T_NUMBER && ret->u.number == 0)
	    return 1;
	if (to->destructed)
	    return 1;
    }
    /*
     * If it is not a room, correct the weight in the 'from' object.
     */
    if (from && from->super && weight) {
	neg_weight.type = T_NUMBER;
	neg_weight.u.number = - weight->u.number;
	(void)apply("add_weight", from, &neg_weight);
    }
    move_object(ob, to);
    return 0;
}

/*
 * Move or destruct one object.
 */
void move_or_destruct(struct object *what, struct object *to)
{
    int res;
    struct value v;

    if (to == 0)
	goto destruct_it;

    res = transfer_object(what, to);
    if (res == 0 || what->destructed)
	return;
    if (res == 1 || res == 4 || res == 5) {
	if (to->super) {
	    move_or_destruct(what, to->super);
	    return;
	}
    }
    /*
     * No need to add the reference count of 'what', as this
     * local 'v' is not deallocated by 'free_all_value()'
     */
 destruct_it:
    v.type = T_OBJECT;
    v.u.ob = what;
    destruct_object(&v);
}

/*
 * Call this one when there is only little memory left. It will start
 * Armageddon.
 */
void slow_shut_down(int minutes)
{
    struct object *ob;
    struct value v;

    /*
     * Swap out objects, and free some memory.
     */
#if 0
#if NUM_RESET_TO_SWAP > 0
    for (ob=obj_list; ob; ob = ob->next_all) {
	if (!ob->swapped)
	    swap(ob);
    }
#endif
#endif
    ob = find_object("obj/shut");
    if (!ob) {
	struct object *save_current = current_object,
	              *save_command = command_giver;
	command_giver = 0;
	current_object = 0;
	shout_string("Game driver shouts: Out of memory.\n");
	command_giver = save_command;
	current_object = save_current;
	startshutdowngame(0);
	return;
    }
    shout_string("Game driver shouts: The memory is getting low !\n");
    v.type = T_NUMBER;
    v.u.number = minutes;
    (void)apply("shut", ob, &v);
}

int match_string(char *match, char *str)
{
    int i;

 again:
    if (*str == '\0' && *match == '\0')
	return 1;
    switch(*match) {
    case '?':
	if (*str == '\0')
	    return 0;
	str++;
	match++;
	goto again;
    case '*':
	match++;
	if (*match == '\0')
	    return 1;
	for (i=0; str[i] != '\0'; i++)
	    if (match_string(match, str+i))
		return 1;
	return 0;
    case '\0':
	return 0;
    case '\\':
	match++;
	if (*match == '\0')
	    return 0;
	/* Fall through ! */
    default:
	if (*match == *str) {
	    match++;
	    str++;
	    goto again;
	}
	return 0;
    }
}
