#include <stdio.h>
#include <setjmp.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdint.h>
#include "lint.h"
#include "y.tab.h"
#include "lnode.h"
#include "interpret.h"
#include "config.h"
#include "object.h"
#include "wiz_list.h"
#ifdef RUSAGE
#include <sys/time.h>
#include <sys/resource.h>
#endif

static struct value *print_function PROT((struct lnode_def *)),
	*print_expr PROT((struct lnode *)),
	*print_expr_list PROT((struct lnode *)),
	*inter_sscanf PROT((struct lnode *)),
	*explode_string PROT((char *, char *));

struct value *get_one_expr PROT((struct lnode **, char *));
extern int transfer_object PROT((struct object *, struct object *)),
	swap PROT((struct object *)),
	remove_call_out PROT((struct object *,  char *)), resort_free_list();

char *string_copy PROT((char *)), *xalloc PROT((size_t)),
	*check_file_name PROT((char *, int));

extern struct value *call_indirect_impl PROT((int, intptr_t, intptr_t, intptr_t)),
    *users();
#define CALL_INDIRECT_1(fun) \
    call_indirect_impl((fun), (intptr_t)0, (intptr_t)0, (intptr_t)0)
#define CALL_INDIRECT_2(fun, a1) \
    call_indirect_impl((fun), (intptr_t)(a1), (intptr_t)0, (intptr_t)0)
#define CALL_INDIRECT_3(fun, a1, a2) \
    call_indirect_impl((fun), (intptr_t)(a1), (intptr_t)(a2), (intptr_t)0)
#define CALL_INDIRECT_4(fun, a1, a2, a3) \
    call_indirect_impl((fun), (intptr_t)(a1), (intptr_t)(a2), (intptr_t)(a3))
#define CALL_INDIRECT_GET(_1, _2, _3, _4, NAME, ...) NAME
#define call_indirect(...) \
    CALL_INDIRECT_GET(__VA_ARGS__, CALL_INDIRECT_4, CALL_INDIRECT_3, \
		      CALL_INDIRECT_2, CALL_INDIRECT_1)(__VA_ARGS__)
extern struct object *previous_ob;
static struct object *frame_ob;
extern char *last_verb;
struct object *current_inherit_ob;

/*
 * Inheritance:
 * An object X can inherit from another object Y. This is done with
 * the statement 'inherit "file";'
 * The inherit statement will clone a copy of that file, call reset
 * in it, and set a pointer to Y from X.
 * Y has to be removed from the linked list of all objects.
 * All variables declared by Y will be copied to X, so that X has access
 * to them.
 *
 * If Y isn't loaded when it is needed, X will be discarded, and Y will be
 * loaded separetly. X will then be reloaded again.
 */
struct lnode_2 *next_arg_list_to_use;

struct lnode_var_def *find_status PROT((char *, int));

extern int tail PROT((char *));

extern void debug_message_value PROT((struct value *)),
	print_local_commands(),
	new_call_out PROT((struct object *, char *, int, struct value *)),
	add_action PROT((char *, char *, int)),
	list_files PROT((char *, char *)),
	enable_commands PROT((int)),
	load_ob_from_swap PROT((struct object *));

extern int d_flag;

extern int current_line, eval_cost;

struct value **current_local_names;

/*
 * This variable is set to true when return is called inside a function,
 * or when the current object is destructed.
 */
static int stop_function;

/*
 * The only reason that the long jump data is in a struct, is that
 * it will be easy to copy. The copy is done so we can stack up
 * several return contexts, and pop back them.
 */
/* struct context {
    jmp_buf a;
    int valid;
};
*/
/*
 * This is used for 'break' in while-statements.
 */
/* struct context break_context; */
int break_flag, break_level;

#ifdef TRACE
struct trace {
    char *function_name;
    char *object_name;
    char *command_name;
    int line;
};

#define MAX_TRACE	30
struct trace trace_back[MAX_TRACE];
int trace_depth = 0;
#endif /* TRACE */

struct value *return_value;	/* Here is a value when "return". */
struct value *current_argument;	/* Arguments to current function */

/*
 * Information about assignments of values:
 *
 * There are three types of l-values: Local variables, global variables
 * and vector elements.
 *
 * The local variables are stored in a vector, with pointers to values
 * of the 'struct value type'. Because they are local, they can all be
 * freed into the free list afterwards. When a local variable is used,
 * the pointer can be directly returned, as these values will never be
 * changed.
 *
 * The global variables must keep their values between executions, and
 * can't use the normal 'struct value', but will use a 'struct svalue' which
 * is smaller also. Because of this, no automatic garbage collection will
 * collect these values when they are overwritten with new values, and
 * thus their values will have to be freed immediately. When a global
 * variable is used, it must be copied into a new 'struct value'.
 *
 * Elements in vectors are similar to global variables. There is a reference
 * count to the whole vector, that states when to deallocate the vector.
 * The elements consists of 'struct svalue's, and will thus have to be freed
 * immediately when over written. When an item in vector is used,
 * it must be copied into a 'struct value'.
 */
 
struct value *make_string(char *str)
{
    struct value *ret = alloc_value();

    ret->type = T_STRING;
    ret->u.string = string_copy(str);
    return ret;
}

struct value *make_number(int n)
{
    struct value *ret = alloc_value();

    ret->type = T_NUMBER;
    ret->u.number = n;
    return ret;
}

struct value *copy_svalue(struct svalue *arg)
{
    struct value *ret;

    switch(arg->type) {
    case T_STRING:
	return make_string(arg->u.string);
    case T_OBJECT:
	if (arg->u.ob->destructed)
	    return &const0;
	ret = alloc_value();
	ret->type = T_OBJECT;
	ret->u.ob = arg->u.ob;
	add_ref(ret->u.ob, "Copy identifier");
	break;
    case T_NUMBER:
	return make_number(arg->u.number);
    case T_POINTER:
	ret = alloc_value();
	ret->type = T_POINTER;
	ret->u.vec = arg->u.vec;
	ret->u.vec->ref++;
	break;
    default:
	return &const0;
    }
    return ret;
}
/*
 * Allocate an array.
 */
struct value *allocate_array(int n)
{
    struct value *v;
    int i;
    struct vector *p;
    if (n < 0 || n > MAX_ARRAY_SIZE)
	error("Illegal array size.\n");
    p = ALLOC_VECTOR(n);
    v = alloc_value();
    v->type = T_POINTER;
    v->u.vec = p;
    p->ref = 1;
    p->size = n;
    p->wl = current_object->wl;
    if (p->wl)
	p->wl->size_array += n;
    for (i=0; i<n; i++) {
	p->item[i].type = T_NUMBER;
	p->item[i].u.number = 0;
    }
    return v;
}

void free_vector(struct vector *p)
{
    int i;
    p->ref--;
    if (p->ref > 0)
	return;
    for (i=0; i<p->size; i++)
	free_svalue(&p->item[i]);
    if (p->wl)
	p->wl->size_array -= p->size;
    free((char *)p);
}

/*
 * Free the data that an svalue is pointing to. Not the svalue
 * itself.
 */
void free_svalue(struct svalue *v)
{
    switch(v->type) {
    case T_STRING:
	free_string(v->u.string);
	break;
    case T_OBJECT:
	free_object(v->u.ob, "free_svalue");
	break;
    case T_POINTER:
	free_vector(v->u.vec);
	break;
    }
}

/*
 * Assign a value to an svalue.
 * This is done either when element in vector, or when to an identifier
 * (as all identifiers are kept in a vector pointed to by the object).
 */

void assign_svalue(struct svalue *dest, struct value *v)
{
    /* First deallocate the previous value. */
    free_svalue(dest);
    if (v == 0) {
	dest->type = T_NUMBER;
	dest->u.number = 0;
	return;
    }
    dest->type = v->type;
    if (v->type == T_STRING)
	dest->u.string = make_shared_string(v->u.string);
    else if (v->type == T_OBJECT) {
	dest->u.ob = v->u.ob;
	add_ref(dest->u.ob, "ass to var");
    } else if (v->type == T_POINTER) {
	dest->u.vec = v->u.vec;
	dest->u.vec->ref++;
    } else
	dest->u.number = v->u.number;
}

/*
 * Check that an assignment to an array item is not cyclic.
 */
static void check_for_recursion(struct vector *vec, struct vector *v)
{
    register int i;
    if (vec->wl)
	vec->wl->cost++;
    eval_cost++;
    if (v == vec)
	error("Recursive asignment of vectors.\n");
    for (i=0; i<v->size; i++) {
	if (v->item[i].type == T_POINTER)
	    check_for_recursion(vec, v->item[i].u.vec);
    }
}

/*
 * Assign a value to a vector element.
 */
void assign_vec(struct lnode *p, struct value *v)
{
    struct value *i, *vec;

    i = print_expr(p->a2);
    vec = print_expr(p->a1);
    if (vec == 0 || vec->type != T_POINTER)
	error("Indexing on illegal type.\n");
    if (v->type == T_POINTER)
	check_for_recursion(vec->u.vec, v->u.vec);
    if (i->type != T_NUMBER || i->u.number < 0 ||
	i->u.number >= vec->u.vec->size)
	error("Illegal index\n");
    assign_svalue(&vec->u.vec->item[i->u.number], v);
}

/*
 * Assign a value to a local variable or global variable.
 */
void assign(struct lnode *where, struct value *v)
{
    struct lnode_number *p;

    if (where->type == F_SUBSCRIPT) {
	assign_vec(where, v);
	return;
    }
    p = (struct lnode_number *)where;
    if (p->type == F_LOCAL_NAME) {
	if (v == 0) {
	    current_local_names[p->number] = &const0;
	    return;
	}
	current_local_names[p->number] = v;
	return;
    } else if (p->type == F_ARGUMENT) {
	fatal("No argument types should exist any longer.\n");
    } else if (p->type == F_IDENTIFIER) {
	struct svalue *dest = find_value((struct lnode_variable *)p);
	assign_svalue(dest, v);
    } else
	error("Assignment to illegal type.\n");
}

static struct value *free_value_list;
static struct value *alloced_value_list;
static int num_alloc;
int tot_alloc_value;

/*
 * Free all values previously allocated.
 */
void free_all_values() {
    for(num_alloc = 0; alloced_value_list; num_alloc++) {
	struct value *tmp;
	tmp = alloced_value_list;
	alloced_value_list = alloced_value_list->next;
	if (tmp->type == T_STRING) {
	    if (!tmp->constant)
		free(tmp->u.string);
	    tmp->u.string = 0;
	} else if (tmp->type == T_OBJECT) {
	    free_object(tmp->u.ob, "free_all_values");
	    tmp->u.ob = 0;
	} else if (tmp->type == T_POINTER) {
	    free_vector(tmp->u.vec);
	    tmp->u.vec = 0;
	}
	tmp->next = free_value_list;
	free_value_list = tmp;
    }
    if (d_flag)
	debug_message("%d alloced values.\n", num_alloc);
}

int count_value_ref(struct object *ob)
{
    int tot = 0;
    struct value *v;
    for(v = alloced_value_list; v; v = v->next) {
	if (v->type == T_OBJECT && v->u.ob == ob)
	    tot++;
    }
    return tot;
}

#define ALLOC_CHUNK	50

struct value *alloc_value() {
    register struct value *tmp;

    if (free_value_list == 0) {
	int i;
	tmp = (struct value *)xalloc(ALLOC_CHUNK * sizeof (struct value));
	tot_alloc_value += ALLOC_CHUNK;
	for (i=0; i < ALLOC_CHUNK - 1; i++)
	    tmp[i].next = &tmp[i+1];
	tmp[ALLOC_CHUNK - 1].next = 0;
	free_value_list = tmp;
    }
    tmp = free_value_list;
    free_value_list = free_value_list->next;
    tmp->next = alloced_value_list;
    alloced_value_list = tmp;
    /*
     * We preset the type to number, to ensure that no one forgets to
     * use the value, as it would have an old, undefined type when arriving
     * to free_all_values().
     */
    tmp->type = T_NUMBER;
    tmp->constant = 0;
    return tmp;
}
/*
 * Print or interpret a function.
 * Set up the long jump context, to enable the 'return' statment.
 * The arguments are passed in one of two ways:
 * Either there is one argument in the variable current_argument. Or there
 * can be a list of (unevaluated) arguments pointed to by next_arg_list_to_use.
 *
 * The reason for this is historical. In the beginning, only one argument
 * could be used, thus only one single pointer to the argument. When
 * the possibility of several arguments was added, this was implemented
 * with an extra info pointer.
 *
 * If the arguments are sent in next_arg_list_to_use, we have to be
 * careful about evaluating them, because current_object points to the
 * new object (if call_other() was used). And the global variables
 * must be fetched from the previous object, and not current_object.
 * This is accomplished by temporarily seting current_object to
 * the previous object, which is stored in frame_ob. frame_ob is set
 * by apply(), even if it is a local function call.
 */

static struct value *print_function(struct lnode_def *p)
{
    struct value **save_current_local_names = current_local_names;
    struct value *new_local_names[MAX_LOCAL];
    struct value *tmp_value;
    int i;
#ifdef TRACE
    int save_trace_depth = trace_depth;
    char *get_current_object_name(), *get_command_giver_name();
#endif

#ifdef TRACE
    if (trace_depth < MAX_TRACE) {
	trace_back[trace_depth].function_name = p->name;
	trace_back[trace_depth].object_name = get_current_object_name();
	trace_back[trace_depth].command_name = get_command_giver_name();
	trace_back[trace_depth].line = -1;
	if (trace_depth > 0)
	    trace_back[trace_depth-1].line = current_line;
	trace_depth++;
    } else {
	error("Too deep recursion.\n");
    }
#endif

    if (p->type == F_IDENTIFIER) {
	    struct lnode_2 *tmp_arg_list;
	struct object *save_current_object = current_object;
	if (d_flag) {
#ifdef TRACE
	    debug_message("%s: ", get_current_object_name());
#endif
	    debug_message("%s(", p->name);
	    debug_message_value(current_argument);
	    debug_message(")\n");
	}
	i = 0;
	if (current_argument) {
	    new_local_names[0] = current_argument;
	    i = 1;
	}
	/* Initialize all arguments. */
	tmp_arg_list = next_arg_list_to_use;
		current_object = frame_ob;
	for (next_arg_list_to_use = 0; i < p->num_arg && tmp_arg_list; i++) {
	    new_local_names[i] = print_expr(tmp_arg_list->expr1);
	    tmp_arg_list = (struct lnode_2 *)tmp_arg_list->expr2;
	}
	current_object = save_current_object;
	/* The remaining arguments and local variables are now initialized. */
	for (; i < p->num_var; i++)
	    new_local_names[i] = &const0;
	current_local_names = new_local_names;
	return_value = &const0;
	(void)print_expr(p->block);
	stop_function = 0;
#ifdef TRACE
	trace_depth = save_trace_depth;
#endif
	current_local_names = save_current_local_names;
	/*
	 * Now clear the return value pointer, so that we won't return
	 * it again somewhere else.
	 */
	tmp_value = return_value;
	return_value = &const0;
	return tmp_value;
    }
    fatal("Illegal type %d\n", p->type);
    return &const0;
}

struct value *catch_value;	/* Used to throw an error to a catch */

/*
 * Evaluate an expression. It is guaranteed that print_expr() never
 * returns a null pointer.
 */
static struct value *print_expr(struct lnode *p)
{
    struct value *ret = &const0;
    struct value *arg, *arg1, *arg2;
    struct lnode_block *lb;
    char *block;
    int i;

    if (const1.u.number != 1)
	fatal("Change in constant 1\n");
    if (const0.u.number != 0)
	fatal("Change in constant 0\n");
    if (current_object && current_object->wl)
	current_object->wl->cost++;
    if (break_flag) return &const0;
    eval_cost++;
    if (eval_cost > MAX_COST) {
	printf("eval_cost to big %d\n", eval_cost);
        eval_cost = 0;
	error("To long evaluation. Execution aborted.\n");
    }
    if (p == 0)
	return &const0;
    current_line = p->line & ~L_MASK;
    switch(p->type) {
    default:
	error("Unimplemented feature (%d)", p->type);
	break;
    case F_CLONE_OBJECT:
	arg1 = print_expr_list(p->a1);
	if (arg1 == 0 || arg1->type != T_STRING) {
	    error("Illegal type argument to clone_object()\n");
	    exit(1);
	}
	ret = call_indirect(F_CLONE_OBJECT, arg1->u.string);
	break;
    case F_AGGREGATE:
    {
	struct lnode *l;
	int num;

	for (num = 0, l = p->a1; l; l = l->a2)
	    num++;
	ret = allocate_array(num);
	for (num = 0, l = p->a1; l; num++, l = l->a2)
	    assign_svalue(&ret->u.vec->item[num], print_expr(l->a1));
	break;
    }
    case F_TAIL:
	arg = print_expr_list(p->a1);
	if (arg == 0 || arg->type != T_STRING)
	    error("Bad argument to tail()\n");
	if (tail(arg->u.string))
	    return &const1;
	return &const0;
    case F_FUNCTION:
	{
	    struct lnode_funcall *f = (struct lnode_funcall *)p;
	    /*
	     * Set the linked list of arguments in a global pointer.
	     * prinf_function() will find it there.
	     * Not a pretty solution, but a simple way of send more than
	     * one argument to functions.
	     */
	    next_arg_list_to_use = (struct lnode_2 *)p->a1;
	    ret = call_indirect(F_FUNCTION, f->name, 0);
	    break;
	}
    case F_SAVE_OBJECT:
	arg = print_expr_list(p->a1);
	if (arg == 0 || arg->type != T_STRING)
	    error("Bad argument to save_object().\n");
	ret = call_indirect(F_SAVE_OBJECT, arg->u.string);
	break;
    case F_FIND_OBJECT:
	arg = print_expr_list(p->a1);
	if (arg == 0 || arg->type != T_STRING)
	    error("Bad argument to find_object().\n");
	ret = call_indirect(F_FIND_OBJECT, arg->u.string);
	break;
    case F_FIND_PLAYER:
    {
	struct object *ob;
	arg = print_expr_list(p->a1);
	if (arg == 0 || arg->type != T_STRING)
	    error("Bad argument to find_player\n");
	ob = find_living_object(arg->u.string, 1);
	if (!ob)
	    break;
	ret = alloc_value();
	ret->type = T_OBJECT;
	ret->u.ob = ob;
	add_ref(ob, "find_player");
	break;
    }
    case F_WRITE_FILE:
    {
	struct lnode *l = p->a1;

	arg = get_one_expr(&l, "write_file");
	if (arg->type != T_STRING)
	    error("Bad first argument to write_file()\n");
	arg1 = get_one_expr(&l, "write_file");
	if (arg1->type != T_STRING)
	    error("Bad second argument to write_file()\n");
	write_file(arg->u.string, arg1->u.string);
	return &const0;
    }
    case F_FILE_SIZE:
	arg = print_expr_list(p->a1);
	if (arg == 0 || arg->type != T_STRING)
	    error("Bad argument to file_size()\n");
	ret = make_number(file_size(arg->u.string));
	break;
    case F_FIND_LIVING:
    {
	struct object *ob;

	arg = print_expr_list(p->a1);
	if (arg == 0 || arg->type != T_STRING)
	    error("Bad argument to find_living\n");
	ob = find_living_object(arg->u.string, 0);
	if (!ob)
	    break;
	ret = alloc_value();
	ret->type = T_OBJECT;
	ret->u.ob = ob;
	add_ref(ob, "find_living");
	break;
    }
    case F_TELL_OBJECT:
	{
	    struct value *who, *what;
	    struct lnode *l = p->a1;

	    who = get_one_expr(&l, "tell_object()");
	    what = get_one_expr(&l, "tell_object()");
	    if (who == 0 || who->type != T_OBJECT)
		error("tell_object() first argument not an object.\n");
	    if (what == 0 || what->type != T_STRING)
		error("tell_object() second argument not a string.\n");
	    (void)call_indirect(F_TELL_OBJECT, who->u.ob, what->u.string);
	}
	break;
    case F_RESTORE_OBJECT:
	arg = print_expr_list(p->a1);
	if (arg == 0 || arg->type != T_STRING)
	    error("Bad argument to restore_object().\n");
	ret = call_indirect(F_RESTORE_OBJECT, arg->u.string);
	break;
    case F_THIS_PLAYER:
	ret = call_indirect(F_THIS_PLAYER);
	break;
    case F_FIRST_INVENTORY:
	arg = print_expr_list(p->a1);
	if (arg == 0 || (arg->type != T_STRING && arg->type != T_OBJECT))
	    error("Bad type argument to first_inventory()\n");
	ret = call_indirect(F_FIRST_INVENTORY, arg);
	break;
    case F_LIVING:
	arg = print_expr_list(p->a1);
	if (arg == 0 || arg->type != T_OBJECT)
	    error("Bad type argument to living()\n");
	ret = call_indirect(F_LIVING, arg->u.ob);
	break;
    case F_CREATOR:
	arg = print_expr_list(p->a1);
	if (arg == 0 || arg->type != T_OBJECT)
	    error("Bad type argument to creator()\n");
	if (arg->u.ob->wl == 0)
	    ret = &const0;
	else
	    ret = make_string(arg->u.ob->wl->name);
	break;
    case F_SHUTDOWN:
	(void)call_indirect(F_SHUTDOWN);
	return &const0;
    case F_EXPLODE:
    {
	struct lnode *l = p->a1;
	arg = get_one_expr(&l, "explode");
	if (arg->type != T_STRING)
	    error("bad type of first argument to explode.\n");
	arg1 = get_one_expr(&l, "explode");
	if (arg1->type != T_STRING)
	    error("bad type of second argument to explode.\n");
	ret = explode_string(arg->u.string, arg1->u.string);
	break;
    }
    case F_FILTER_OBJECTS:
    {
	struct lnode *l = p->a1;

	arg = get_one_expr(&l, "filter_obects");
	if (arg->type != T_POINTER)
	    error("Bad first argument to filter_obects()\n");
	arg1 = get_one_expr(&l, "filter_obects");
	if (arg1->type != T_STRING)
	    error("Bad second argument to filter_obects()\n");
	arg2 = get_one_expr(&l, "filter_obects");
	if (arg2->type != T_OBJECT)
	    error("Bad third argument to filter_obects()\n");
	ret = filter(arg, arg1->u.string, arg2->u.ob);
	break;
    }
    case F_SET_BIT:
    {
	struct lnode *l = p->a1;
	char *str;
	int len, old_len, ind;

	arg  = get_one_expr(&l, "set_bit");
	if (arg->type != T_STRING)
	    error("Bad first argument to set_bit\n");
	arg1 = get_one_expr(&l, "set_bit");
	if (arg1->type != T_NUMBER)
	    error("Bad second argument to set_bit\n");
	if (arg1->u.number > MAX_BITS)
	    error("set_bit: too big bit number: %d\n", arg1->u.number);
	len = strlen(arg->u.string);
	old_len = len;
	ind = arg1->u.number/6;
	if (ind >= len)
	    len = ind + 1;
	str = xalloc(len+1);
	str[len] = '\0';
	if (old_len)
	    memcpy(str, arg->u.string, old_len);
	if (len > old_len)
	    memset(str + old_len, ' ', len - old_len);
	if (str[ind] > 0x3f + ' ' || str[ind] < ' ')
	    error("Illegal bit pattern in set_bit character %d\n", ind);
	str[ind] = (str[ind] - ' ' | 1 << arg1->u.number % 6) + ' ';
	ret = alloc_value();
	ret->type = T_STRING;
	ret->u.string = str;
	break;
    }
    case F_CLEAR_BIT:
    {
	struct lnode *l = p->a1;
	char *str;
	int len, ind;

	arg  = get_one_expr(&l, "clear_bit");
	if (arg->type != T_STRING)
	    error("Bad first argument to clear_bit\n");
	arg1 = get_one_expr(&l, "clear_bit");
	if (arg1->type != T_NUMBER)
	    error("Bad second argument to clear_bit\n");
	if (arg1->u.number > MAX_BITS)
	    error("clear_bit: too big bit number: %d\n", arg1->u.number);
	len = strlen(arg->u.string);
	ind = arg1->u.number/6;
	if (ind >= len) {
	    ret = arg;
	    break;
	}
	str = xalloc(len+1);
	memcpy(str, arg->u.string, len+1);	/* Including null byte */
	if (str[ind] > 0x3f + ' ' || str[ind] < ' ')
	    error("Illegal bit pattern in clear_bit character %d\n", ind);
	str[ind] = (str[ind] - ' ' & ~(1 << arg1->u.number % 6)) + ' ';
	ret = alloc_value();
	ret->type = T_STRING;
	ret->u.string = str;
	break;
    }
    case F_TEST_BIT:
    {
	struct lnode *l = p->a1;
	int len;

	arg  = get_one_expr(&l, "test_bit");
	if (arg->type != T_STRING)
	    error("Bad first argument to test_bit\n");
	arg1 = get_one_expr(&l, "test_bit");
	if (arg1->type != T_NUMBER)
	    error("Bad second argument to test_bit\n");
	len = strlen(arg->u.string);
	if (arg1->u.number/6 >= len)
	    break;
	if (arg->u.string[arg1->u.number/6] - ' ' & 1 << arg1->u.number % 6)
	    ret = &const1;
	break;
    }
    case F_QUERY_LOAD_AVERAGE:
    {
	ret = query_load_av();
	break;
    }
    case F_CATCH:
	/*
	 * Catch/Throw - catch errors in system or other peoples routines.
	 */
    {
	extern jmp_buf error_recovery_context;
	extern int error_recovery_context_exists;
	extern struct value * catch_value;
   	jmp_buf old_error_context;
	int old_exists_flag;
	struct object *save_commander = command_giver,
	    *save_current = current_object;
	
	memcpy((char *) old_error_context,
	       (char *) error_recovery_context,
	       sizeof error_recovery_context);
	old_exists_flag = error_recovery_context_exists;
	
   	catch_value = &const0; /* system errors throw a string */
   	
	/* signal catch OK - print no err msg */
   	error_recovery_context_exists = 2;
	if (setjmp(error_recovery_context)) {
	    /* they did a throw() or error */
	    ret = catch_value;		/* get whatever they threw */
	    current_object = save_current;
	    command_giver = save_commander;
	} else {
	    (void)print_expr_list(p->a1);	/* do it */
	    ret = &const0;		/* no error - catch returns zero */
	}

	/* restore enclosing contexts */
	memcpy((char *) error_recovery_context,
	       (char *) old_error_context,
	       sizeof error_recovery_context);
	error_recovery_context_exists = old_exists_flag;
	/* next error will still return 1 by default */
	catch_value = &const1;
	break;
    }
    case F_THROW:
    {
	catch_value = print_expr_list(p->a1);
	if (catch_value != 0 && catch_value->type == T_NUMBER &&
	  catch_value->u.number == 0)
	    catch_value = &const1;
	throw_error(); /* do the longjump, with extra checks... */
	break;
    }
    case F_NOTIFY_FAIL:
	arg = print_expr_list(p->a1);
	if (arg == 0 || arg->type != T_STRING)
	    error("Bad argument to notify_fail\n");
	set_notify_fail_message(arg->u.string);
	return &const0;
    case F_QUERY_IDLE:
    {
	extern int query_idle PROT((struct object *));
	arg = print_expr_list(p->a1);
	if (!arg || arg->type != T_OBJECT)
	    error("Bad argument to query_idle().\n");
	ret = make_number(query_idle(arg->u.ob));
	break;
    }
    case F_IMPLODE:
    {
	struct value *implode_string PROT((struct vector *, char *));
	struct lnode *l = p->a1;
	arg = get_one_expr(&l, "implode");
	if (arg->type != T_POINTER)
	    error("bad type of first argument to implode.\n");
	arg1 = get_one_expr(&l, "implode");
	if (arg1->type != T_STRING)
	    error("bad type of second argument to implode.\n");
	ret = implode_string(arg->u.vec, arg1->u.string);
	break;
    }
    case F_QUERY_SNOOP:
	if (command_giver == 0)
	    return &const0;
	arg = print_expr_list(p->a1);
	if (arg == 0 || arg->type != T_OBJECT)
	    error("Bad argument to query_snoop\n");
	if (arg->u.ob->interactive == 0)
	    return &const0;
	arg1 = apply("query_level", command_giver, 0);
	if (arg1->type != T_NUMBER || arg1->u.number < 22)
	    return &const0;
	ret = query_snoop(arg->u.ob);
	break;
    case F_QUERY_IP_NUMBER:
    {
	extern char *query_ip_number PROT((struct object *));
	char *tmp;

	if (p->a1)
	    arg = print_expr_list(p->a1);
	else
	  arg = 0;
	if (arg && arg->type != T_OBJECT)
	    error("Bad optional argument to query_ip_number()\n");
	tmp = query_ip_number(arg ? arg->u.ob : 0);
	if (tmp == 0)
	    ret = &const0;
	else
	    ret = make_string(tmp);
	break;
    }
    case F_QUERY_HOST_NAME:
    {
	extern char *query_host_name();
	char *tmp;

	tmp = query_host_name();
	ret = alloc_value();
	ret->type = T_STRING;
	ret->constant = 1;
	ret->u.string = tmp;
	break;
    }
    case F_NEXT_INVENTORY:
	arg = print_expr_list(p->a1);
	if (arg == 0 || (arg->type != T_STRING && arg->type != T_OBJECT))
	    error("Bad type argument to next_inventory()\n");
	ret = call_indirect(F_NEXT_INVENTORY, arg);
	break;
    case F_ENVIRONMENT:
	arg = print_expr_list(p->a1);
	if (arg && arg->type != T_STRING && arg->type != T_OBJECT)
	    error("Wrong type to optional arg to environment()\n");
	ret = call_indirect(F_ENVIRONMENT, arg);
	break;
    case F_THIS_OBJECT:
	ret = call_indirect(F_THIS_OBJECT);
	break;
    case F_PREVIOUS_OBJECT:
	if (previous_ob == 0) {
	    ret = &const0;
	    break;
	}
	ret = alloc_value();
	ret->type = T_OBJECT;
	ret->u.ob = previous_ob;
	add_ref(previous_ob, "previous_object()");
	break;
    case F_PEOPLE:
	(void)call_indirect(F_PEOPLE);
	return &const0;
    case F_LOCALCMD:
	print_local_commands();
	return &const0;
    case F_SWAP:
	arg = print_expr_list(p->a1);
	if (arg == 0 || arg->type != T_OBJECT)
	    error("bad argument to swap()\n");
	(void)swap(arg->u.ob);
	return &const0;
    case F_TIME:
	ret = make_number(time(0l));
	break;
    case F_WIZLIST:
    {
	struct lnode *l = p->a1;
	arg = get_one_expr(&l, 0);
	if (arg && arg->type != T_STRING)
	    error("Bad argument to wizlist()\n");
	(void)wizlist(arg);
	return &const0;
    }
    case F_TRANSFER:
    {
	struct object *dest;
	struct lnode *l = p->a1;
	int i;
	arg = get_one_expr(&l, "transfer");
	if (arg->type != T_OBJECT)
	    error("Bad type of artgument 1 to transfer()\n");
	arg1 = get_one_expr(&l, "transfer");
	if (arg1->type != T_OBJECT && arg1->type != T_STRING)
	    error("Bad type of artgument 2 to transfer()\n");
	if (arg1->type == T_STRING) {
	    dest = find_object(arg1->u.string);
	    if (dest == 0)
		error("Object not found.\n");
	} else {
	    dest = arg1->u.ob;
	}
	i = transfer_object(arg->u.ob, dest);
	if (i == 0)
	    ret = &const0;	/* Usually this */
	else if (i == 1)
	    ret = &const1;
	else
	    ret = make_number(i);
    }
	break;
    case F_ADD_WORTH:
    {
	struct lnode *l = p->a1;
	arg = get_one_expr(&l, "add_worth");
	if (arg->type != T_NUMBER)
	    error("Bad type of argument 1 to add_worth()\n");
	arg1 = get_one_expr(&l, 0);
	if (arg1 != 0 && arg1->type != T_OBJECT)
	    error("Bad type of artgument 2 to add_worth()\n");
	if (strncmp(current_object->name, "obj/", 4) != 0 &&
	    strncmp(current_object->name, "room/", 5) != 0)
	    error("Illegal call of add_worth.\n");
	if (arg1) {
	    if (arg1->u.ob->wl)
		arg1->u.ob->wl->total_worth += arg->u.number;
	    return &const0;
	}
	if (previous_ob == 0)
	    return &const0;
	if (previous_ob->wl)
	    previous_ob->wl->total_worth += arg->u.number;
    }
	return &const0;
    case F_ADD:
	ret = alloc_value();
	arg1 = print_expr(p->a1);
	arg2 = print_expr(p->a2);
	if (arg1->type == T_STRING && arg2->type == T_STRING) {
	    ret->u.string = xalloc(strlen(arg1->u.string) +
				   strlen(arg2->u.string) + 1);
	    (void)strcpy(ret->u.string, arg1->u.string);
	    (void)strcat(ret->u.string, arg2->u.string);
	    ret->type = T_STRING;
	    break;
	}
	if (arg1->type == T_NUMBER && arg2->type == T_STRING) {
	    char buff[20];
	    sprintf(buff, "%d", arg1->u.number);
	    ret->type = T_STRING;
	    ret->u.string = xalloc(strlen(arg2->u.string) +
				   strlen(buff) + 1);
	    strcpy(ret->u.string, buff);
	    strcat(ret->u.string, arg2->u.string);
	    break;
	}
	if (arg2->type == T_NUMBER && arg1->type == T_STRING) {
	    char buff[20];
	    sprintf(buff, "%d", arg2->u.number);
	    ret->type = T_STRING;
	    ret->u.string = xalloc(strlen(arg1->u.string) +
				   strlen(buff) + 1);
	    strcpy(ret->u.string, arg1->u.string);
	    strcat(ret->u.string, buff);
	    break;
	}
	if (arg1->type == T_NUMBER && arg2->type == T_NUMBER) {
	    ret->u.number = arg1->u.number + arg2->u.number;
	    ret->type = T_NUMBER;
	    break;
	}
	error("Bad type of arg to '+'\n");
	exit(1);
    case F_SUBTRACT:
	arg1 = print_expr(p->a1);
	arg2 = print_expr(p->a2);
	if (arg1->type != T_NUMBER || arg2->type != T_NUMBER) {
	    error("Bad type on arg to '-'\n");
	    exit(1);
	}
	ret = make_number(arg1->u.number - arg2->u.number);
	break;
    case F_AND:
	arg1 = print_expr(p->a1);
	arg2 = print_expr(p->a2);
	if (arg1->type != T_NUMBER || arg2->type != T_NUMBER) {
	    error("Bad type on arg to '&'\n");
	    exit(1);
	}
	ret = make_number(arg1->u.number & arg2->u.number);
	break;
    case F_OR:
	arg1 = print_expr(p->a1);
	arg2 = print_expr(p->a2);
	if (arg1->type != T_NUMBER || arg2->type != T_NUMBER) {
	    error("Bad type on arg to '|'\n");
	    exit(1);
	}
	ret = make_number(arg1->u.number | arg2->u.number);
	break;
    case F_XOR:
	arg1 = print_expr(p->a1);
	arg2 = print_expr(p->a2);
	if (arg1->type != T_NUMBER || arg2->type != T_NUMBER) {
	    error("Bad type on arg to '^'\n");
	    exit(1);
	}
	ret = make_number(arg1->u.number ^ arg2->u.number);
	break;
    case F_LSH:
	arg1 = print_expr(p->a1);
	arg2 = print_expr(p->a2);
	if (arg1->type != T_NUMBER || arg2->type != T_NUMBER) {
	    error("Bad type on arg to '<<'\n");
	    exit(1);
	}
	ret = make_number(arg1->u.number << arg2->u.number);
	break;
    case F_RSH:
	arg1 = print_expr(p->a1);
	arg2 = print_expr(p->a2);
	if (arg1->type != T_NUMBER || arg2->type != T_NUMBER) {
	    error("Bad type on arg to '>>'\n");
	    exit(1);
	}
	ret = make_number(arg1->u.number >> arg2->u.number);
	break;
    case F_LAND:
	arg1 = print_expr(p->a1);
	if (arg1->type == T_NUMBER && arg1->u.number == 0)
	    break;
	arg2 = print_expr(p->a2);
	if (arg2->type == T_NUMBER && arg2->u.number == 0)
	    break;
	ret = &const1;
	break;
    case F_LOR:
	ret = print_expr(p->a1);
	if ((ret->type == T_NUMBER && ret->u.number != 0) ||
		    ret->type != T_NUMBER)
	    break;
	ret = print_expr(p->a2);
	if ((ret->type == T_NUMBER && ret->u.number != 0) ||
		    ret->type != T_NUMBER)
	    break;
	ret = &const0;
	break;
    case F_MULTIPLY:
	arg1 = print_expr(p->a1);
	arg2 = print_expr(p->a2);
	if (arg1->type != T_NUMBER || arg2->type != T_NUMBER) {
	    error("Bad type on arg to '*'\n");
	    exit(1);
	}
	ret = make_number(arg1->u.number * arg2->u.number);
	break;
    case F_DIVIDE:
	arg1 = print_expr(p->a1);
	arg2 = print_expr(p->a2);
	if (arg1->type != T_NUMBER || arg2->type != T_NUMBER) {
	    error("Bad type on arg to '/'\n");
	    exit(1);
	}
	if (arg2->u.number == 0)
	    error("Division by zero.\n");
	ret = make_number(arg1->u.number / arg2->u.number);
	break;
    case F_MOD:
	arg1 = print_expr(p->a1);
	arg2 = print_expr(p->a2);
	if (arg1->type != T_NUMBER || arg2->type != T_NUMBER) {
	    error("Bad type on arg to '%'\n");
	    exit(1);
	}
	if (arg2->u.number == 0)
	    error("Modulus by zero.\n");
	ret = make_number(arg1->u.number % arg2->u.number);
	break;
    case F_GT:
	arg1 = print_expr(p->a1);
	arg2 = print_expr(p->a2);
	if (arg1->type == T_STRING && arg2->type == T_STRING) {
	    if (strcmp(arg1->u.string, arg2->u.string) > 0)
	        ret = &const1;
	    else
	        ret = &const0;
	    break;
	} else if (arg1->type != T_NUMBER || arg2->type != T_NUMBER) {
	    error("Bad type on arg to '>'\n");
	    exit(1);
	}
	if (arg1->u.number > arg2->u.number)
	    ret = &const1;
	else
	    ret = &const0;
	break;
    case F_GE:
	arg1 = print_expr(p->a1);
	arg2 = print_expr(p->a2);
	if (arg1->type == T_STRING && arg2->type == T_STRING) {
	    if (strcmp(arg1->u.string, arg2->u.string) >= 0)
	        ret = &const1;
	    else
	        ret = &const0;
	    break;
	} else if (arg1->type != T_NUMBER || arg2->type != T_NUMBER) {
	    error("Bad type on arg to '>='\n");
	    exit(1);
	}
	if (arg1->u.number >= arg2->u.number)
	    ret = &const1;
	else
	    ret = &const0;
	break;
    case F_LT:
	arg1 = print_expr(p->a1);
	arg2 = print_expr(p->a2);
	if (arg1->type == T_STRING && arg2->type == T_STRING) {
	    if (strcmp(arg1->u.string, arg2->u.string) < 0)
	        ret = &const1;
	    else
	        ret = &const0;
	    break;
	} else if (arg1->type != T_NUMBER || arg2->type != T_NUMBER) {
	    error("Bad type on arg to '<'\n");
	    exit(1);
	}
	if (arg1->u.number < arg2->u.number)
	    ret = &const1;
	else
	    ret = &const0;
	break;
    case F_LE:
	arg1 = print_expr(p->a1);
	arg2 = print_expr(p->a2);
	if (arg1->type == T_STRING && arg2->type == T_STRING) {
	    if (strcmp(arg1->u.string, arg2->u.string) <= 0)
	        ret = &const1;
	    else
	        ret = &const0;
	    break;
	} else if (arg1->type != T_NUMBER || arg2->type != T_NUMBER) {
	    error("Bad type on arg to '<='\n");
	    exit(1);
	}
	if (arg1->u.number <= arg2->u.number)
	    ret = &const1;
	else
	    ret = &const0;
	break;
    case F_EQ:
	arg1 = print_expr(p->a1);
	arg2 = print_expr(p->a2);
	ret = alloc_value();
	ret->type = T_NUMBER;
	if (arg1->type == T_NUMBER && arg2->type == T_NUMBER) {
	    ret->u.number = arg1->u.number == arg2->u.number;
	} else if (arg1->type == T_POINTER && arg2->type == T_POINTER) {
	    ret->u.number = arg1->u.vec == arg2->u.vec;
	} else if (arg1->type == T_STRING && arg2->type == T_STRING) {
	    ret->u.number = !strcmp(arg1->u.string, arg2->u.string);
	} else if (arg1->type == T_OBJECT && arg2->type == T_OBJECT) {
	    ret->u.number = arg1->u.ob == arg2->u.ob;
	} else
	    ret->u.number = 0;
	break;
    case F_NE:
	arg1 = print_expr(p->a1);
	arg2 = print_expr(p->a2);
	if (arg1->type != arg2->type) {
	    ret = &const1;
	    break;
	}
	ret = alloc_value();
	ret->type = T_NUMBER;
	if (arg1->type == T_NUMBER) {
	    ret->u.number = arg1->u.number != arg2->u.number;
	} else if (arg1->type == T_STRING) {
	    ret->u.number = strcmp(arg1->u.string, arg2->u.string);
	} else if (arg1->type == T_POINTER) {
	    ret->u.number = arg1->u.vec != arg2->u.vec;
	} else if (arg1->type == T_OBJECT) {
	    ret->u.number = arg1->u.ob != arg2->u.ob;
	} else
	    ret->u.number = 0;
	break;
    case F_BLOCK:	/* Sequential list of statements. */
    {
	lb = (struct lnode_block *)p;
	block = lb->block;
	
		for (i=0; i < lb->num_nodes && !stop_function && !break_flag; i++) {
		    if (current_object && current_object->destructed &&
		      ((struct lnode *)block)->type != F_RETURN) {
			stop_function = 1;
			return_value = &const0;
		    }
	    print_expr((struct lnode *)block);
	    block += lnode_size[((struct lnode *)block)->line >> L_SHIFT];
	}
	return &const0;
    }
	    case F_CONS:	/* Linked list of statements. */
		while(p && !stop_function && !break_flag) {
		    if (current_object && current_object->destructed &&
			p->a1->type != F_RETURN) {
			return_value = &const0;
			stop_function = 1;
		    }
	    (void)print_expr(p->a1);
	    p = p->a2;
	}
	return &const0;
	    case F_COMMA:
		while(p && !stop_function && !break_flag) {
		    if (current_object && current_object->destructed &&
			p->a1->type != F_RETURN) {
			return_value = &const0;
			stop_function = 1;
		    }
	    ret = print_expr(p->a1);
	    p = p->a2;
	}
	break;
    case F_IF:
	ret = print_expr(p->a1);
	if ((ret->type == T_NUMBER && ret->u.number) || ret->type != T_NUMBER)
	    (void)print_expr(p->a2);
	else if (p->a3)
	    (void)print_expr(p->a3);
	return &const0;
    case F_ARGUMENT:
	if (current_argument == 0) {
	    ret = &const0;
	    break;
	}
	ret = current_argument;
	if (ret->type == T_OBJECT && ret->u.ob->destructed)
	    ret = &const0;
	break;
    case F_IDENTIFIER:
	ret = copy_svalue(find_value((struct lnode_variable *)p));
	break;
    case F_RETURN:
	if (p->a1)
	    return_value = print_expr(p->a1);
	else
	    return_value = &const0;
	stop_function = 1;
	return &const0;
    case F_BREAK:
	if (!break_level)
	    error("Illegal break statement!\n");
/*	longjmp(break_context.a, 1);
	fatal("Return from longjmp\n"); */
	break_flag = 1;
	return &const0;
    case F_CONTINUE:
	if (!break_level)
	    error("Illegal continue statement!\n");
/*	longjmp(break_context.a, 2);
	fatal("Return from longjmp\n"); */
	break_flag=2;
	return &const0;
    case F_LOG_FILE:
        {
	    struct lnode *l;
	    l = p->a1;
	    arg1 = get_one_expr(&l, "log_file");
	    arg2 = get_one_expr(&l, "log_file");
	    if (arg1 == 0 || arg1->type != T_STRING ||
		arg2 == 0 || arg2->type != T_STRING)
		error("Bad type argument to log_file().\n");
	    (void)call_indirect(F_LOG_FILE, arg1->u.string, arg2->u.string);
	}
	return &const0;
    case F_NOT:
	arg = print_expr(p->a1);
	if (arg->type == T_NUMBER && arg->u.number == 0)
	    ret = &const1;
	else
	    ret = &const0;
	break;
    case F_COMPL:
	arg = print_expr(p->a1);
	if (arg->type != T_NUMBER)
	    error("Bad argument to '~'\n");
	ret = make_number(~ arg->u.number);
	break;
    case F_NEGATE:
	arg = print_expr(p->a1);
	if (arg->type != T_NUMBER)
	    error("Bad argument to unary '-'\n");
	ret = make_number(- arg->u.number);
	break;
    case F_INC:
	arg = print_expr(p->a1);
	if (arg->type != T_NUMBER)
	    error("Bad argument to '++' (pre)\n");
	ret = make_number(arg->u.number+1);
	assign(p->a1,ret);
	break;
    case F_DEC:
	arg = print_expr(p->a1);
	if (arg->type != T_NUMBER)
	    error("Bad argument to '--' (pre)\n");
	ret = make_number(arg->u.number-1);
	assign(p->a1,ret);
	break;
    case F_POST_INC:
	ret = print_expr(p->a1);
	if (ret->type != T_NUMBER)
	    error("Bad argument to '++' (post)\n");
	arg = make_number(ret->u.number+1);
	assign(p->a1,arg);
	break;
    case F_POST_DEC:
	ret = print_expr(p->a1);
	if (ret->type != T_NUMBER)
	    error("Bad argument to '--' (post)\n");
	arg = make_number(ret->u.number-1);
	assign(p->a1,arg);
	break;
    case F_CALL_OTHER:
	{
	    struct value *a1, *a2;
	    struct lnode *l = p->a1;
	    a1 = get_one_expr(&l, "call_other");
	    a2 = get_one_expr(&l, "call_other");
	    /*
	     * Send the rest of the arguments in the linked list,
	     * unevaluated. It is thus possible to send more than one
	     * argument.
	     */
	    /* a3 = get_one_expr(&l, (char *)0); */
	    next_arg_list_to_use = (struct lnode_2 *)l;
	    if (a1 == 0 || (a1->type != T_STRING &&
			    a1->type != T_OBJECT)) {
		error("Wrong type arg 1 to call_other()\n");
		exit(1);
	    }
	    if (a2->type != T_STRING) {
		error("Wrong type arg 2 to call_other()\n");
		exit(1);
	    }
	    ret = call_indirect(F_CALL_OTHER, a1, a2->u.string, 0);
	}
	break;
    case F_INTP:
	arg = print_expr_list(p->a1);
	if (arg && arg->type == T_NUMBER)
	    ret = &const1;
	else
	    ret = &const0;
	break;
    case F_STRINGP:
	arg = print_expr_list(p->a1);
	if (arg && arg->type == T_STRING)
	    ret = &const1;
	else
	    ret = &const0;
	break;
    case F_OBJECTP:
	arg = print_expr_list(p->a1);
	if (arg && arg->type == T_OBJECT)
	    ret = &const1;
	else
	    ret = &const0;
	break;
    case F_POINTERP:
	arg = print_expr_list(p->a1);
	if (arg && arg->type == T_POINTER)
	    ret = &const1;
	else
	    ret = &const0;
	break;
    case F_EXTRACT:
    {
	struct lnode *l;
	int len, from, to;

	l = p->a1;
	arg = get_one_expr(&l, "extract");
	if (arg->type != T_STRING)
	    error("Bad first argument to extract()\n");
	len = strlen(arg->u.string);
	arg1 = get_one_expr(&l, 0);
	if (!arg1) {
	    ret = arg;
	    break;
	}
	if (arg1->type != T_NUMBER)
	    error("Bad second argument to extract()\n");
	from = arg1->u.number;
	if (from < 0)
	    error("Bad second argument to extract()\n");
	ret = alloc_value();
	ret->type = T_STRING;
	if (from >= len) {
	    ret->u.string = "";
	    ret->constant = 1;
	    break;
	}
	arg2 = get_one_expr(&l, 0);
	if (!arg2) {
	    ret->u.string = arg->u.string + from;
	    ret->constant = 1;
	    break;
	}
	if (arg2->type != T_NUMBER)
	    error("Bad third argument to extract()\n");
	to = arg2->u.number;
	if (to < from) {
	    ret->u.string = "";
	    ret->constant = 1;
	    break;
	}
	if (to >= len)
	    to = len-1;
	if (to == len-1) {
	    ret->u.string = arg->u.string + from;
	    ret->constant = 1;
	    break;
	}
	ret->u.string = xalloc(to - from + 2);
	strncpy(ret->u.string, arg->u.string + from,
		to - from + 1);
	ret->u.string[to - from + 1] = '\0';
	break;
    }
    case F_QUERY_VERB:
	if (last_verb == 0) {
	    ret = &const0;
	    break;
	}
	ret = alloc_value();
	ret->type = T_STRING;
	ret->constant = 1;
	ret->u.string = last_verb;
	break;
    case F_FILE_NAME:
	arg = print_expr_list(p->a1);
	if (arg->type != T_OBJECT)
	    error("Bad argument to file_name()\n");
	ret = alloc_value();
	ret->type = T_STRING;
	ret->u.string = arg->u.ob->name;
	ret->constant = 1;
	break;
    case F_USERS:
	ret = users();
	break;
    case F_CALL_OUT:
	{
	    struct lnode *l = p->a1;
	    arg = get_one_expr(&l, "call_out");
	    arg1 = get_one_expr(&l, "call_out");
	    arg2 = get_one_expr(&l, 0);
	    if (arg->type != T_STRING) {
		error("Wrong type arg 1 to call_out()\n");
		exit(1);
	    }
	    if (arg1->type != T_NUMBER) {
		error("Wrong type arg 2 to call_out()\n");
		exit(1);
	    }
	    new_call_out(current_object, arg->u.string, arg1->u.number, arg2);
	    return &const0;
	}
    case F_REMOVE_CALL_OUT:
	{
	    struct lnode *l = p->a1;
	    arg = get_one_expr(&l, "call_out");
	    if (arg->type != T_STRING) {
		error("Wrong type arg 1 to remove_call_out()\n");
		exit(1);
	    }
	    ret = make_number(remove_call_out(current_object, arg->u.string));
	    break;
	}
    case F_WRITE:
	arg = print_expr_list(p->a1);
	if (arg == 0)
	    error("Bad argument to write()");
	(void)call_indirect(F_WRITE, arg);
	return &const0;
#if 0
    case F_REGCOMP:
	arg = print_expr_list(p->a1);
	if (arg == 0 || arg->type != T_STRING)
	    error("Bad argument to regcomp()");
	ret = alloc_value();
	ret->type = T_POINTER;
	ret->u.vec = (struct vector *)xalloc(sizeof(struct vector_regexp));
	ret->u.vec =
	    (char *)regcomp(arg->u.string);
	break;
    case F_REGEXEC:
    {
	struct lnode *l = p->a1;
	arg1 = get_one_expr(&l, "regexec");
	arg2 = get_one_expr(&l, "regexec");
	if (arg1 == 0 || arg2->type != T_STRING)
	    error("Bad arg 1 to regexec.\n");
	if (arg2 == 0 || arg2->type != T_STRING)
	    error("Bad arg 2 to regexec.\n");
	debug_message("regexec: arg1 u.string: %x\n",arg1->u.string);
	debug_message("regexec: arg2 u.string: %x\n",arg2->u.string);
	if (regexec(arg1->u.string, arg2->u.string))
	    ret = &const1;
	else
	    ret = &const0;
    }
#endif
    case F_MOVE_OBJECT:
	{
	    struct lnode *l = p->a1;
	    arg1 = get_one_expr(&l, "move_object");
	    arg2 = get_one_expr(&l, "move_object");
	    if (arg1 == 0 || (arg1->type != T_STRING &&
			      arg1->type != T_OBJECT)) {
		error("Bad type argument to move_object()\n");
		return &const0;
	    }
	    if (arg2 == 0 || (arg2->type != T_STRING &&
			      arg2->type != T_OBJECT)) {
		error("Bad type argument to move_object()\n");
		return &const0;
	    }
	    (void)call_indirect(F_MOVE_OBJECT, arg1, arg2);
	    return &const0;
	}
    case F_SNOOP:
	{
	    struct lnode *l = p->a1;
	    arg1 = get_one_expr(&l, (char *)0);
	    if (arg1 && arg1->type != T_OBJECT)
		error("Bad type arg to snoop.\n");
	    (void)call_indirect(F_SNOOP, arg1 ? arg1->u.ob : 0);
	    return &const0;
	}
    case F_ADD_ACTION:
	{
	    struct lnode *l = p->a1;
	    arg1 = get_one_expr(&l, "add_action");
	    if (arg1 == 0 || arg1->type != T_STRING) {
		error("Bad type argument 1 to add_action()\n");
		exit(1);
	    }
	    arg2 = get_one_expr(&l, 0);
	    if (arg2 != 0 && arg2->type != T_STRING) {
		error("Bad type argument 2 to add_action()\n");
		exit(1);
	    }
	    if (arg2) {
		arg = get_one_expr(&l, 0);
		if (arg && arg->type != T_NUMBER)
		    error("Bad argument number 3 to add_action.\n");
	    } else
		arg = 0;
	    add_action(arg1->u.string, arg2 ? arg2->u.string : 0,
				arg ? arg->u.number : 0);
	}
	return &const0;
    case F_ADD_VERB:
	ret = print_expr_list(p->a1);
	if (ret == 0 || ret->type != T_STRING) {
	    error("Bad type argument to add_verb()\n");
	    exit(1);
	}
	(void)call_indirect(F_ADD_VERB, ret->u.string);
	return &const0;
    case F_ALLOCATE:
	ret = print_expr_list(p->a1);
	if (ret == 0 || ret->type != T_NUMBER) {
	    error("Bad type argument to allocate()\n");
	    exit(1);
	}
	ret = allocate_array(ret->u.number);
	break;
    case F_ED:
	arg = print_expr_list(p->a1);
	if (arg == 0) {
	    char *p;
	    if (command_giver == 0 || command_giver->interactive == 0)
		return &const0;
	    arg = apply("query_real_name", command_giver, 0);
	    if (arg == 0 || arg->type != T_STRING)
		return &const0;
	    p = get_error_file(arg->u.string);
	    if (p == 0)
		return &const0;
	    (void)call_indirect(F_ED, p);
	    return &const0;
	}
	if (arg->type != T_STRING)
	    error("Bad argument to ed().\n");
	(void)call_indirect(F_ED, arg ? arg->u.string : 0);
	return &const0;
    case F_CRYPT:
    {
	char salt[2];
	char *choise =
	    "abcdefghijklmnopqrstuvxyzABCDEFGHIJKLMNOPQRSTUVXYZ0123456789./";
	struct lnode *l = p->a1;
	arg = get_one_expr(&l, "crypt");
	if (arg->type != T_STRING)
	    error("Bad type first arg to crypt.\n");
	arg1 = get_one_expr(&l, "crypt");
	if (arg1->type == T_STRING && strlen(arg->u.string) >= 2) {
	    salt[0] = arg1->u.string[0];
	    salt[1] = arg1->u.string[1];
	} else {
#ifdef RANDOM
	    salt[0] = choise[random() % strlen(choise)];
	    salt[1] = choise[random() % strlen(choise)];
#else /* RANDOM */
#ifdef DRAND48
	    salt[0] = choise[(int)(drand48() * strlen(choise))];
	    salt[1] = choise[(int)(drand48() * strlen(choise))];
#else /* DRAND48 */
	    salt[0] = choise[time(0l) % strlen(choise)];
	    salt[1] = choise[time(0l) % strlen(choise)];
#endif /* DRAND48 */
#endif /* RANDOM */
	}
	ret = alloc_value();
	ret->type = T_STRING;
#ifdef sun
	ret->u.string = string_copy(_crypt(arg->u.string, salt));
#else
	ret->u.string = string_copy(crypt(arg->u.string, salt));
#endif
	break;
    }
    case F_CREATE_WIZARD:
	arg = print_expr_list(p->a1);
	if (arg == 0 || arg->type != T_STRING)
	    error("Bad argument to create_wizard().\n");
	ret = call_indirect(F_CREATE_WIZARD, arg->u.string);
	break;
    case F_DESTRUCT:
	ret = print_expr_list(p->a1);
		if (ret == 0 || (ret->type != T_STRING && ret->type != T_OBJECT)) {
	    error("Bad type argument to destruct()\n");
	    exit(1);
	}
	(void)call_indirect(F_DESTRUCT, ret);
	return &const0;
    case F_RANDOM:
	arg = print_expr_list(p->a1);
	if (arg == 0 || arg->type != T_NUMBER)
	    error("Bad type arg to random()\n");
	if (arg->u.number <= 0) {
	    ret = &const0;
	    break;
	}
#ifdef DRAND48
	ret = make_number((int)(drand48() * arg->u.number));
#else
#ifdef RANDOM
	ret = make_number(random() % arg->u.number);
#else
	ret = make_number((((int)arg + (int) ret)>>2 + time(0l)) %
	    arg->u.number);
#endif /* RANDOM */
#endif /* DRAND48 */
	break;
    case F_SAY:
	{
	    struct lnode *l = p->a1;
	    arg1 = get_one_expr(&l, "say()");
	    if (arg1 == 0) {
		error("Bad type argument to say()\n");
		exit(1);
	    }
	    arg2 = get_one_expr(&l, (char *)0);
	    if (arg2 && arg2->type != T_OBJECT)
		error("Bad type argument to say()\n");
	    (void)call_indirect(F_SAY, arg1, arg2 ? arg2->u.ob : 0);
	}
	return &const0;
    case F_TELL_ROOM:
	{
	    struct lnode *l = p->a1;
		    struct object *ob = 0;

	    arg1 = get_one_expr(&l, "tell_room");
	    if (arg1->type == T_OBJECT)
		ob = arg1->u.ob;
	    else if (arg1->type != T_STRING)
		error("Bad type argument to tell_room()\n");
	    else {
		ob = find_object(arg1->u.string);
		if (ob == 0)
		    error("Object not found.\n");
	    }
	    arg2 = get_one_expr(&l, "tell_room");
	    (void)call_indirect(F_TELL_ROOM, ob, arg2);
	}
	return &const0;
    case F_SHOUT:
	ret = print_expr_list(p->a1);
	if (ret == 0 || ret->type != T_STRING) {
	    error("Bad type argument to shout()\n");
	    exit(1);
	}
	(void)call_indirect(F_SHOUT, ret->u.string);
	return &const0;
    case F_WHILE:
	{
/*	    struct context old_context;

	    old_context = break_context; */
	    /*
	     * We come to the next statement in three ways:
	     * 0: Set up of long jump.
	     * 1: break statement.
	     * 2: continue statement.
	     */
/*	    if (setjmp(break_context.a) == 1) {
		break_context.valid = 0;
		break;
	    } */
	    break_level++;
	    while(!stop_function && !break_flag) {
		arg = print_expr(p->a1);
		if (arg->type == T_NUMBER && arg->u.number == 0)
		    break;
		(void)print_expr(p->a2);
		if(break_flag==2) break_flag=0;
		if (current_object->destructed) {
		    return_value = &const0;
		    stop_function = 1;
		}
	    }
	    break_flag=0;
	    break_level--;
	    return &const0;
	}
    case F_DO:
	{
/*	    struct context old_context;

	    old_context = break_context; */
	    /*
	     * We come to the next statement in three ways:
	     * 0: Set up of long jump.
	     * 1: break statement.
	     * 2: continue statement.
	     */
/*	    if (setjmp(break_context.a) == 1) {
		break_context.valid = 0;
		break;
	    } */
	    break_level++;
	    while(!stop_function && !break_flag) {
		(void)print_expr(p->a1);
		if(break_flag==2) break_flag=0;
		arg = print_expr(p->a2);
		if (arg->type == T_NUMBER && arg->u.number == 0)
		    break;
		if (current_object->destructed) {
		    return_value = &const0;
		    stop_function = 1;
		}
	    }
	    break_flag=0;
	    break_level--;
	    return &const0;
	}
    case F_FOR:
	{
/*	    struct context old_context;

	    old_context = break_context; */
	    /*
	     * We come to the next statement in three ways:
	     * 0: Set up of long jump.
	     * 1: break statement.
	     * 2: continue statement.
	     */
/*	    if (setjmp(break_context.a) == 1) {
		break_context.valid = 0;
		break;
	    } */
	    break_level++;
	    while(!stop_function && !break_flag) {
		arg = print_expr(p->a1);
		if (arg->type == T_NUMBER && arg->u.number == 0)
		    break;
		(void)print_expr(p->a3);
		if(break_flag==2) break_flag=0;
		if(!break_flag) (void)print_expr(p->a2);
		if (current_object->destructed) {
		    return_value = &const0;
		    stop_function = 1;
		}
	    }
	    break_flag=0;
	    break_level--;
	    return &const0;
	}
    case F_SUBSCRIPT:
	arg1 = print_expr(p->a1);
	arg2 = print_expr(p->a2);
	if ((arg1->type != T_STRING  && arg1->type != T_POINTER) ||
	    arg2->type != T_NUMBER)
	    error("Bad type argument to subscripts.\n");
	if (arg1->type == T_POINTER) {
	    i = arg2->u.number;
	    if (i < 0 || i >= arg1->u.vec->size)
		error("Illegal index size: %d\n", i);
	    ret = copy_svalue(&arg1->u.vec->item[i]);
	    break;
	}
		if ((size_t)arg2->u.number >= strlen(arg1->u.string))
		    break;
	ret = alloc_value();
	ret->type = T_NUMBER;
	ret->u.number = arg1->u.string[arg2->u.number];
	break;
    case F_STRLEN:
	arg = print_expr_list(p->a1);
	if (arg == 0 || arg->type != T_STRING)
	    error("Illegal argument to strlen()\n");
	ret = alloc_value();
	ret->type = T_NUMBER;
	ret->u.number = strlen(arg->u.string);
	break;
    case F_SIZEOF:
	arg = print_expr_list(p->a1);
	if (arg == 0 || arg->type != T_POINTER)
	    error("Illegal argument to sizeof()\n");
	ret = alloc_value();
	ret->type = T_NUMBER;
	ret->u.number = arg->u.vec->size;
	break;
    case F_LOWER_CASE:
	arg = print_expr_list(p->a1);
	if (arg == 0 || arg->type != T_STRING)
	    error("Bad type argument to lowercase\n");
	{
	    int i;
	    ret = make_string(arg->u.string);
	    for (i = strlen(ret->u.string)-1; i>=0; i--)
		if (isalpha(ret->u.string[i]))
		    ret->u.string[i] |= 'a' - 'A';
	}
	break;
    case F_SET_HEART_BEAT:
	arg = print_expr_list(p->a1);
	if (arg == 0 || arg->type != T_NUMBER)
	    error("Bad type argument to set_heart_beat()\n");
	ret = call_indirect(F_SET_HEART_BEAT, arg->u.number);
	break;
    case F_CAPITALIZE:
	ret = print_expr_list(p->a1);
	if (ret == 0 || ret->type != T_STRING)
	    error("Bad type arg to capitalize\n");
	if (ret->u.string[0] == '\0')
	    break;
	if (islower(ret->u.string[0])) {
	    ret = make_string(ret->u.string);
	    ret->u.string[0] += 'A' - 'a';
	}
	break;
    case F_COMMAND:
    {
	struct lnode *l = p->a1;
	arg = get_one_expr(&l, "command");
	if (arg->type != T_STRING) {
	    error("Bad first argument to command()\n");
	    exit(1);
	}
	arg1 = get_one_expr(&l, 0);
	if (arg1 && arg1->type != T_OBJECT)
	    error("Bad second argument to command()\n");
	ret = call_indirect(F_COMMAND, arg->u.string, arg1 ? arg1->u.ob : 0);
	break;
    }
    case F_LS:
    {
	struct lnode *l = p->a1;
	arg = get_one_expr(&l, "ls");
	if (arg->type != T_STRING && arg->type != T_NUMBER)
	    error("Bad type argument to command()\n");
	arg1 = get_one_expr(&l, 0);
	if (arg1 && arg1->type != T_STRING)
	    error("Bad second argument to ls()\n");
	list_files(arg->type == T_STRING ? arg->u.string : 0,
		   arg1 ? arg1->u.string : 0);
	return &const0;
    }
    case F_RM:
	ret = print_expr_list(p->a1);
	if (ret == 0 || ret->type != T_STRING) {
	    error("Bad type argument to rm()\n");
	    exit(1);
	}
	(void)call_indirect(F_RM, ret->u.string);
	return &const0;
    case F_CAT:
    {
	struct lnode *l = p->a1;
	int start = 0, len = 0;
	ret = get_one_expr(&l, "cat");
	if (ret == 0 || ret->type != T_STRING) {
	    error("Bad type argument to cat()\n");
	    exit(1);
	}
	arg1 = get_one_expr(&l, 0);
	if (arg1) {
	    if (arg1->type != T_NUMBER)
		error("Bad second argument to cat\n");
	    start = arg1->u.number;
	    arg2 = get_one_expr(&l, "cat third arg");
	    if (arg2->type != T_NUMBER)
		error("Bad third argument to cat\n");
	    len = arg2->u.number;
	}
	ret = call_indirect(F_CAT, ret->u.string, start, len);
    }
	break;
    case F_MKDIR:
    {
	struct lnode *l = p->a1;
	char *path;
	ret = get_one_expr(&l, "mkdir");
	if (ret->type != T_STRING)
	    error("Bad type argument to mkdir()\n");
	path = check_file_name(ret->u.string, 1);
	if (path == 0 || mkdir(path, 0770) == -1)
	    ret = &const0;
	else
	    ret = &const1;
    }
	break;
    case F_RMDIR:
    {
	struct lnode *l = p->a1;
	char *path;
	ret = get_one_expr(&l, "mkdir");
	if (ret->type != T_STRING)
	    error("Bad type argument to rmdir()\n");
	path = check_file_name(ret->u.string, 1);
	if (path == 0 || rmdir(path) == -1)
	    ret = &const0;
	else
	    ret = &const1;
    }
	break;
    case F_INPUT_TO:
	{
	    struct lnode *l = p->a1;
	    int flag = 1;
	    ret = get_one_expr(&l, "input_to");
	    if (ret == 0 || ret->type != T_STRING) {
		error("Bad type argument to input_to()\n");
		exit(1);
	    }
	    arg1 = get_one_expr(&l, 0);
	    if (!arg1 || (arg1->type == T_NUMBER && arg1->u.number == 0))
		flag = 0;
	    ret = call_indirect(F_INPUT_TO, ret->u.string, flag);
	}
	break;
    case F_SET_LIVING_NAME:
	arg = print_expr_list(p->a1);
	if (arg == 0 || arg->type != T_STRING)
	    error("Bad argument to set_living_name()\n");
	set_living_name(current_object, arg->u.string);
	return &const0;
    case F_PARSE_COMMAND:
	ret = parse(p->a1);
	break;
    case F_SSCANF:
	ret = inter_sscanf(p->a1);
	break;
    case F_ENABLE_COMMANDS:
	(void)call_indirect(F_ENABLE_COMMANDS);
	return &const0;
    case F_DISABLE_COMMANDS:
	enable_commands(0);
	return &const0;
    case F_PRESENT:
	{
	    struct lnode *l = p->a1;
	    arg1 = get_one_expr(&l, "present()");
	    if (arg1->type != T_STRING && arg1->type != T_OBJECT)
		error("Bad type argument to present()\n");
	    arg2 = get_one_expr(&l, NULL);
	    if (arg2 && arg2->type != T_OBJECT)
		error("Bad second argument to present()\n");
	    ret = call_indirect(F_PRESENT, arg1, arg2 ? arg2->u.ob : 0);
	}
	break;
    case F_SET_LIGHT:
	arg = print_expr_list(p->a1);
	if (arg == 0 || arg->type != T_NUMBER)
	    error("Bad type argument to set_light()\n");
	ret = call_indirect(F_SET_LIGHT, arg->u.number);
	break;
    case F_CONST0:
	ret = &const0;
	break;
    case F_CONST1:
	ret = &const1;
	break;
    case F_NUMBER:
    {
	struct lnode_number *np = (struct lnode_number *)p;
	ret = alloc_value();
	ret->type = T_NUMBER;
	ret->u.number = np->number;
    }
	break;
    case F_ASSIGN:
	ret = print_expr(p->a2);
	assign(p->a1, ret);
	break;
    case F_CTIME:
    {
	long tmp;
	char *cp;
	arg = print_expr_list(p->a1);
	if (arg == 0 || arg->type != T_NUMBER)
	    error("Bad argument to ctime()\n");
	tmp = arg->u.number;
	ret = make_string(ctime(&tmp));
	/* Now strip the newline. */
	cp = strchr(ret->u.string, '\n');
	if (cp)
	    *cp = '\0';
    }
	break;
    case F_ADD_EQ:
        {
	    struct value *v;
	    arg = print_expr(p->a2);
	    v = print_expr(p->a1);
	    if (v->type == T_STRING) {
			char *new_str = 0;
		if (arg->type == T_STRING) {
		    new_str = xalloc(strlen(arg->u.string) +
			strlen(v->u.string) + 1);
		    strcpy(new_str, v->u.string);
		    strcat(new_str, arg->u.string);
		} else if (arg->type == T_NUMBER) {
		    char buff[20];
		    sprintf(buff, "%d", arg->u.number);
		    new_str = xalloc(strlen(v->u.string) + strlen(buff) + 1);
		    strcpy(new_str, v->u.string);
		    strcat(new_str, buff);
			} else {
			    error("Illegal rhs to +=\n");
			}
			if (new_str == 0) {
			    ret = &const0;
			    break;
			}
			ret = alloc_value();
		ret->type = T_STRING;
		ret->u.string = new_str;
		assign(p->a1, ret);
		break;
	    }
	    if (arg->type != T_NUMBER)
		error("Bad type to rhs +=.\n");
	    if (v->type != T_NUMBER)
		error("Bad type to lhs +=");
	    assign(p->a1, ret = make_number(v->u.number + arg->u.number));
	}
	break;
    case F_SUB_EQ:
	{
	    struct value *v;
	    arg = print_expr(p->a2);
	    if (arg->type != T_NUMBER)
		error("Bad type to -=.\n");
	    v = print_expr(p->a1);
	    if (v->type != T_NUMBER)
		error("Bad type to -=");
	    assign(p->a1, ret = make_number(v->u.number - arg->u.number));
	}
	break;
    case F_MULT_EQ:
	{
	    struct value *v;
	    arg = print_expr(p->a2);
	    if (arg->type != T_NUMBER)
		error("Bad type rhs to *=.\n");
	    v = print_expr(p->a1);
	    if (v->type != T_NUMBER)
		error("Bad type lhs to *=");
	    assign(p->a1, ret = make_number(v->u.number * arg->u.number));
	}
	break;
    case F_AND_EQ:
	{
	    struct value *v;
	    arg = print_expr(p->a2);
	    if (arg->type != T_NUMBER)
		error("Bad type to &=.\n");
	    v = print_expr(p->a1);
	    if (v->type != T_NUMBER)
		error("Bad type to &=");
	    assign(p->a1, ret = make_number(v->u.number & arg->u.number));
	}
	break;
    case F_OR_EQ:
	{
	    struct value *v;
	    arg = print_expr(p->a2);
	    if (arg->type != T_NUMBER)
		error("Bad type to |=.\n");
	    v = print_expr(p->a1);
	    if (v->type != T_NUMBER)
		error("Bad type to |=");
	    assign(p->a1, ret = make_number(v->u.number | arg->u.number));
	}
	break;
    case F_XOR_EQ:
	{
	    struct value *v;
	    arg = print_expr(p->a2);
	    if (arg->type != T_NUMBER)
		error("Bad type to ^=.\n");
	    v = print_expr(p->a1);
	    if (v->type != T_NUMBER)
		error("Bad type to ^=");
	    assign(p->a1, ret = make_number(v->u.number ^ arg->u.number));
	}
	break;
    case F_LSH_EQ:
	{
	    struct value *v;
	    arg = print_expr(p->a2);
	    if (arg->type != T_NUMBER)
		error("Bad type to <<=.\n");
	    v = print_expr(p->a1);
	    if (v->type != T_NUMBER)
		error("Bad type to <<=");
	    assign(p->a1, ret = make_number(v->u.number << arg->u.number));
	}
	break;
    case F_RSH_EQ:
	{
	    struct value *v;
	    arg = print_expr(p->a2);
	    if (arg->type != T_NUMBER)
		error("Bad type to >>=.\n");
	    v = print_expr(p->a1);
	    if (v->type != T_NUMBER)
		error("Bad type to >>=");
	    assign(p->a1, ret = make_number(v->u.number >> arg->u.number));
	}
	break;
    case F_COMBINE_FREE_LIST:
#ifdef MALLOC_malloc
	ret = make_number(resort_free_list());
#else
	ret = &const0;
#endif
	break;
    case F_LOCAL_NAME:
	ret = current_local_names[((struct lnode_number *)p)->number];
	if (ret->type == T_OBJECT && ret->u.ob->destructed)
	    ret = &const0;
	break;
    case F_DIV_EQ:
	{
	    struct value *v;
	    arg = print_expr(p->a2);
	    if (arg->type != T_NUMBER)
		error("Bad type rhs to /=.\n");
	    v = print_expr(p->a1);
	    if (v->type != T_NUMBER)
		error("Bad type lhs to /=");
	    if (arg->u.number == 0)
		error("Division by zero.");
	    assign(p->a1, ret = make_number(v->u.number / arg->u.number));
	}
	break;
    case F_MOD_EQ:
	{
	    struct value *v;
	    arg = print_expr(p->a2);
	    if (arg->type != T_NUMBER)
		error("Bad type rhs to %=.\n");
	    v = print_expr(p->a1);
	    if (v->type != T_NUMBER)
		error("Bad type lhs to %=");
	    if (arg->u.number == 0)
		error("Division by zero.");
	    assign(p->a1, ret = make_number(v->u.number % arg->u.number));
	}
	break;
    case F_STRING:
	{
	    struct lnode_name *n = (struct lnode_name *)p;
	    ret = alloc_value();
	    ret->u.string = n->name;
	    ret->constant = 1;
	    ret->type = T_STRING;
	    break;
	}
    case F_RUSAGE:
    {
        char buff[500];

#ifdef RUSAGE
	struct rusage rus;
	long utime, stime;
	int maxrss;

	if (getrusage(RUSAGE_SELF, &rus) < 0)
            buff[0] = 0;
	else {
	    utime = rus.ru_utime.tv_sec * 1000 + rus.ru_utime.tv_usec / 1000;
	    stime = rus.ru_stime.tv_sec * 1000 + rus.ru_stime.tv_usec / 1000;
	    maxrss = rus.ru_maxrss;
#ifdef sun
	    maxrss *= getpagesize() / 1024;
#endif
	    sprintf(buff, "%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld",
		    utime, stime, (long)maxrss, (long)rus.ru_ixrss, (long)rus.ru_idrss,
		    (long)rus.ru_isrss, (long)rus.ru_minflt, (long)rus.ru_majflt, (long)rus.ru_nswap,
		    (long)rus.ru_inblock, (long)rus.ru_oublock, (long)rus.ru_msgsnd,
		    (long)rus.ru_msgrcv, (long)rus.ru_nsignals, (long)rus.ru_nvcsw,
		    (long)rus.ru_nivcsw);
	  }
#else /* RUSAGE */
	buff[0] = 0;
#endif /* RUSAGE */
	ret = make_string(buff);
	break;
    }
    }
    if (ret == 0)
	ret = &const0;
    return ret;
}

static struct value *print_expr_list(struct lnode *p)
{
    if (p == 0)
	return 0;
    if (p->type != F_CONS)
	fatal("Bad type to print_expr_list()\n");
    return print_expr(p->a1);
}

/*
 * Apply a fun 'fun' to the program in object 'ob', with the
 * optional argument 'arg'.
 * If the function is not found, search in the object pointed to by the
 * inherit pointer.
 * If the function name starts with '::', search in the object pointed out
 * through the inherit pointer by the current object. The 'current_object'
 * stores the base object, not the object that has the current function being
 * evaluated. Thus, the variable current_inherit_ob is used to save this
 * information.
 *
 * There is a special case when called from the heart beat, as
 * current_inherit_ob will be 0. So when it is 0, we set current_inherit_ob
 * to the 'ob' sent as argument.
 *
 * If the function is not found, return a null pointer which indicates
 * this. print_function() will never return a null pointer.
 *
 * Note that the object 'ob' can be destructed. This must be handled by
 * the caller of apply().
 */

char debug_apply_fun[30]; /* For debugging */

struct value *apply(char *fun, struct object *ob, struct value *arg)
{
    struct lnode_def *pr;
    struct value *old_argument = current_argument;
    struct object *save_current_object = current_object;
    struct object *prog_ob;
    extern int num_error;
    struct object *save_current_inherit_ob;
    int length;

    strncpy(debug_apply_fun, fun, sizeof debug_apply_fun);
    if (num_error > 0)
	return &const0;
    prog_ob = ob;
    if (fun[0] == ':' && fun[1] == ':') {
	if (current_object != ob)
	    error("Illegal to call functions with '::' from the outside.\n");
	/* current_inherit_ob can only be 0 when called from heart beat. */
	if (current_inherit_ob != 0)
	    prog_ob = current_inherit_ob;
    }
    length = strlen(fun);
    for (; prog_ob; prog_ob = prog_ob->inherit) {
	if (fun[0] == ':' && fun[1] == ':') {
	    fun += 2;
	    length -= 2;	/* Must recompute the length without '::' */
	    continue;
	}
	if (prog_ob->swapped)
	    load_ob_from_swap(prog_ob);
	pr = prog_ob->prog;
	if (pr == 0)
	    continue;
	if (prog_ob->destructed) {
	    current_object = prog_ob;
	    error("Executing '%s' in destructed object.\n", fun);
	}
	for(; pr; pr = pr->next) {
	    if (pr->type == F_IDENTIFIER && pr->length == length &&
	      strcmp(pr->name, fun) == 0) {
		struct value *ret;
		struct object *save_previous_ob;
		/* Static functions may not be called from outside. */
		if (pr->is_static && current_object != ob)
		    continue;
		current_argument = arg;
		frame_ob = current_object;
		save_previous_ob = previous_ob;
		if (current_object != ob)
		    previous_ob = current_object;
		current_object = ob;
		save_current_inherit_ob = current_inherit_ob;
		current_inherit_ob = prog_ob;
		ret = print_function(pr);
		current_inherit_ob = save_current_inherit_ob;
		/*
		 * print_function() will clear the value of
		 * next_arg_list_to_use
		 */
		current_object = save_current_object;
		previous_ob = save_previous_ob;
		current_argument = old_argument;
		return ret;
	    }
	}
    }
    if (d_flag)
	debug_message("--Could not find the function %s\n", fun);
    /*
     * We must clear next_arg_list_to_use, so that those arguments won't be
     * sent to another function.
     */
    next_arg_list_to_use = 0;
    return 0;
}

/*
 * Call a specific function in an object.
 * Make sure that current_object is set up, and the the program
 * is not swapped.
 */
void call_function(struct lnode_def *pr)
{
    (void)print_function(pr);
}

#ifdef TRACE
/*
 * Write out a trace. If there is an heart_beat(), then return the
 * object that had that heart beat.
 */
char *dump_trace() {
    int i;
    char *ret = 0;

    if (trace_depth == 0) {
	(void)printf("No trace.\n");
	debug_message("No trace.\n");
	return 0;
    }
    for (i=0; i<trace_depth; i++) {
	(void)printf("%-3d: '%20s' in '%20s' line %d\n", i,
		     trace_back[i].function_name, trace_back[i].object_name,
		     trace_back[i].line);
	debug_message("%-3d: '%20s' in '%20s' line %d\n", i,
		      trace_back[i].function_name, trace_back[i].object_name,
		      trace_back[i].line);
	if (strcmp(trace_back[i].function_name, "heart_beat") == 0)
	    ret = trace_back[i].object_name;
    }
    return ret;
}
#endif /* TRACE */

struct value *get_one_expr(struct lnode **pp, char *from)
{
    struct value *v;

    if (*pp == 0) {
	if (from == 0)
	    return 0;
	error("Wrong number of arguments to %s", from);
    }
    v = print_expr((*pp)->a1);
    *pp = (*pp)->a2;
    return v;
}

static char *find_percent(char *str)
{
    while(1) {
	str = strchr(str, '%');
	if (str == 0)
	    return 0;
	if (str[1] != '%')
	    return str;
	str++;
    }
}

static struct value *inter_sscanf(struct lnode *l)
{
    struct value *p;
    char *fmt;		/* Format description */
    char *in_string;	/* The string o be parsed. */
    int number_of_matches;
    char *cp;

    /*
     * First get the string to be parsed.
     */
    p = get_one_expr(&l, "sscanf");
    if (p == 0 || p->type != T_STRING)
	error("Bad first argument to sscanf.");
    in_string = p->u.string;
    if (in_string == 0)
	return &const0;
    /*
     * Now get the format description.
     */
    p = get_one_expr(&l, "sscanf");
    if (p == 0 || p->type != T_STRING)
	error("Bad second argument to sscanf.");
    fmt = p->u.string;
    if (fmt == 0)
	return &const0;
    /*
     * First, skip and match leading text.
     */
    for (cp=find_percent(fmt); fmt != cp; fmt++, in_string++) {
	if (in_string[0] == '\0' || fmt[0] != in_string[0])
	    return &const0;
    }
    /*
     * For every % or substring in the format.
     */
    for (number_of_matches=0; l && l->a1; number_of_matches++, l = l->a2) {
	int i, type;

	if (fmt[0] == '\0') {
	    /*
	     * We have reached end of the format string.
	     * If there are any chars left in the in_string,
	     * then we put them in the last variable (if any).
	     */
	    if (in_string[0]) {
		assign(l->a1, make_string(in_string));
		number_of_matches++;
	    }
	    break;
	}
	if (fmt[0] != '%')
	    fatal("Should be a %% now !\n");
	type = T_STRING;
	if (fmt[1] == 'd')
	    type = T_NUMBER;
	else if (fmt[1] != 's')
	    error("Bad type : '%%%c' in sscanf fmt string.", fmt[1]);
	fmt += 2;
	/*
	 * Parsing a number is the easy case. Just use strtol() to
	 * find the end of the number.
	 */
	if (type == T_NUMBER) {
	    char *tmp = in_string;
	    int tmp_num;

	    tmp_num = (int) strtol(in_string, &in_string, 0);
	    if(tmp == in_string) {
		/* No match */
		break;
	    }
	    assign(l->a1, make_number(tmp_num));
	    while(fmt[0] && fmt[0] == in_string[0])
		fmt++, in_string++;
	    if (fmt[0] != '%') {
		number_of_matches++;
		break;
	    }
	    continue;
	}
	/*
	 * Now we have the string case.
	 */
	cp = find_percent(fmt);
	if (cp == fmt)
	    error("Illegal to have 2 adjacent %'s in fmt string in sscanf.");
	if (cp == 0)
	    cp = fmt + strlen(fmt);
	/*
	 * First case: There was no extra characters to match.
	 * Then this is the last match.
	 */
	if (cp == fmt) {
	    assign(l->a1, make_string(in_string));
	    number_of_matches++;
	    break;
	}
	for (i=0; in_string[i]; i++) {
	    if (strncmp(in_string+i, fmt, cp - fmt) == 0) {
		char *match;
		/*
		 * Found a match !
		 */
		match = xalloc(i+1);
		(void)strncpy(match, in_string, i);
		in_string += i + cp - fmt;
		match[i] = '\0';
		assign(l->a1, make_string(match));
		free(match);
		fmt = cp;	/* Advance fmt to next % */
		break;
	    }
	}
	if (fmt == cp)	/* If match, then do continue. */
	    continue;
	/*
	 * No match was found. Then we stop here, and return
	 * the result so far !
	 */
	break;
    }
    return make_number(number_of_matches);
}

struct value *users() {
    struct object *ob;
    extern int num_player; /* set by comm1.c */
    int i;
    struct value *ret;

    ret = allocate_array(num_player);
    for (i = 0; i < num_player; i++) {
	ret->u.vec->item[i].type = T_OBJECT;
	ret->u.vec->item[i].u.ob = ob = get_interactive_object(i);
	add_ref(ob, "users");
    }
    return ret;
}

static struct value *explode_string(char *str, char *del)
{
    char *p, *beg;
    int num, len;
    struct value *ret;
    char *buff;
    len = strlen(del);
    /*
     * Skip leading 'del' strings, if any.
     */
    while(strncmp(str, del, len) == 0) {
	str += len;
	if (str[0] == '\0')
	    return &const0;
    }
    /*
     * Find number of occurences of the delimiter 'del'.
     */
    for (p=str, num=0; *p;) {
	if (strncmp(p, del, len) == 0) {
	    num++;
	    p += len;
	} else
	    p += 1;
    }
    /*
     * Compute number of array items. It is either number of delimiters,
     * or, one more.
     */
	    if (strlen(str) < (size_t)len ||
		strcmp(str + strlen(str) - len, del) != 0)
	num++;
    buff = xalloc(strlen(str) + 1);
    ret = allocate_array(num);
    for (p=str, beg = str, num=0; *p; ) {
	if (strncmp(p, del, len) == 0) {
	    strncpy(buff, beg, p - beg);
	    buff[p-beg] = '\0';
	    if (num >= ret->u.vec->size)
		fatal("Too big index in explode !\n");
	    assign_svalue(&ret->u.vec->item[num], make_string(buff));
	    num++;
	    beg = p + len;
	    p = beg;
	} else {
	    p += 1;
	}
    }
    /* Copy last occurence, if there was not a 'del' at the end. */
    if (*beg != '\0')
	assign_svalue(&ret->u.vec->item[num], make_string(beg));
    free(buff);
    return ret;
}

struct value *implode_string(struct vector *arr, char *del)
{
    int size, i, num;
    char *p, *q;
    struct value *ret;

    for (i=0, size = 0, num = 0; i < arr->size; i++) {
	if (arr->item[i].type == T_STRING) {
	    size += strlen(arr->item[i].u.string);
	    num++;
	}
    }
    if (num == 0)
	return &const0;
    p = xalloc(size + (num-1) * strlen(del) + 1);
    q = p;
    p[0] = '\0';
    for (i=0, num=0; i < arr->size; i++) {
	if (arr->item[i].type == T_STRING) {
	    if (num > 0) {
		strcpy(p, del);
		p += strlen(del);
	    }
	    strcpy(p, arr->item[i].u.string);
	    p += strlen(arr->item[i].u.string);
	    num++;
	}
    }
    ret = alloc_value();
    ret->type = T_STRING;
    ret->u.string = q;
    return ret;
}
