#include <string.h>
#include "lint.h"
#include "y.tab.h"
#include "lnode.h"
#include "config.h"

#define NUM_CHUNK		100	/* Number of chunks to allocate */

extern char *xalloc(size_t);
extern int current_line;
int tot_alloc_lnode;

struct lnode_var_def *prog_status;

void free_lnode PROT((struct lnode *, int)),
	free_lnode_string PROT((struct lnode *));
void free_sub_part PROT((struct lnode *, int));
static int tot_lnode_single;
struct lnode_single *alloc_lnode_single(int type)
{
    struct lnode_single *p;
    p = (struct lnode_single *)xalloc(sizeof (struct lnode_single));
    p->type = type;
    p->line = current_line + L_SINGLE;
    tot_lnode_single++;
    return p;
}

static struct lnode_number *lnode_number_list;
static int tot_lnode_number;
struct lnode_number *alloc_lnode_number(int type, int number)
{
    struct lnode_number *p;
    if (!lnode_number_list) {
	int i;
	lnode_number_list = (struct lnode_number *)xalloc(NUM_CHUNK *
					sizeof (struct lnode_number));
	for (p = lnode_number_list, i=0; i<NUM_CHUNK - 1; p++, i++)
	    p->next = p+1;
	p->next = 0;
	tot_lnode_number += NUM_CHUNK;
    }
    p = lnode_number_list;
    lnode_number_list = lnode_number_list->next;
    p->type = type;
    p->number = number;
    p->next = 0;
    p->line = current_line + L_NUMBER;
    return p;
}

/*
 * Strings are allocated from the shared string area.
 * Be carfule not to free() such an area through free().
 */
static struct lnode_name *lnode_name_list;
static int tot_lnode_name;
struct lnode_name *alloc_lnode_name(int type, char *name)
{
    struct lnode_name *p;
    if (!lnode_name_list) {
	int i;
	lnode_name_list = (struct lnode_name *)xalloc(NUM_CHUNK *
						sizeof (struct lnode_name));
	for (p = lnode_name_list, i=0; i<NUM_CHUNK - 1; p++, i++)
	    p->name = (char *)(p+1);
	p->name = 0;
	tot_lnode_name += NUM_CHUNK;
    }
    p = lnode_name_list;
    lnode_name_list = (struct lnode_name *)lnode_name_list->name;
    p->type = type;
    p->name = make_shared_string(name);
    free(name);
    p->line = current_line + L_NAME;
    return p;
}

static int tot_lnode_variable;
struct lnode_variable *alloc_lnode_variable(int type, int number)
{
    struct lnode_variable *p;
    p = (struct lnode_variable *)xalloc(sizeof (struct lnode_variable));
    p->type = type;
    p->number = number;
    p->line = current_line + L_VARIABLE;
    /* p->is_static = 0; */
    tot_lnode_variable++;
    return p;
}

static struct lnode_1 *lnode_1_list;
static int tot_lnode_1;
struct lnode_1 *alloc_lnode_1(int type, struct lnode *expr)
{
    struct lnode_1 *p;
    if (!lnode_1_list) {
	int i;
	lnode_1_list = (struct lnode_1 *)xalloc(NUM_CHUNK *
						sizeof (struct lnode_1));
	for (p = lnode_1_list, i=0; i<NUM_CHUNK - 1; p++, i++)
	    p->expr = (struct lnode *)(p+1);
	p->expr = 0;
	tot_lnode_1 += NUM_CHUNK;
    }
    p = lnode_1_list;
    lnode_1_list = (struct lnode_1 *)lnode_1_list->expr;
    p->type = type;
    p->expr = expr;
    p->line = current_line + L_1;
    return p;
}

static struct lnode_2 *lnode_2_list;
static int tot_lnode_2;
struct lnode_2 *alloc_lnode_2(int type, struct lnode *expr1,
    struct lnode *expr2)
{
    struct lnode_2 *p;
    if (!lnode_2_list) {
	int i;
	lnode_2_list = (struct lnode_2 *)xalloc(NUM_CHUNK *
						sizeof (struct lnode_2));
	for (p = lnode_2_list, i=0; i<NUM_CHUNK - 1; p++, i++)
	    p->expr1 = (struct lnode *)(p+1);
	p->expr1 = 0;
	tot_lnode_2 += NUM_CHUNK;
    }
    p = lnode_2_list;
    lnode_2_list = (struct lnode_2 *)lnode_2_list->expr1;
    p->type = type;
    p->line = current_line + L_2;
    p->expr1 = expr1;
    p->expr2 = expr2;
    return p;
}

static int tot_lnode_3;
struct lnode_3 *alloc_lnode_3(int type, struct lnode *expr1,
    struct lnode *expr2, struct lnode *expr3)
{
    struct lnode_3 *p;
    p = (struct lnode_3 *)xalloc(sizeof (struct lnode_3));
    p->type = type;
    p->line = current_line + L_3;
    p->expr1 = expr1;
    p->expr2 = expr2;
    p->expr3 = expr3;
    tot_lnode_3++;
    return p;
}

static int tot_lnode_def;
struct lnode_def *alloc_lnode_def(int type, char *name, struct lnode *block,
    int num_var)
{
    struct lnode_def *p;
    p = (struct lnode_def *)xalloc(sizeof (struct lnode_def));
    p->type = type;
    p->line = current_line + L_DEF;
    p->name = make_shared_string(name);
    free(name);
    p->length = strlen(p->name);
    p->num_var = num_var;
    p->block = block;
    p->next = 0;
    p->num_ref = 0;
    p->is_static = 0;
    p->num_arg = 0;
    tot_lnode_def++;
    return p;
}

static int tot_var_def;
void alloc_lnode_var_def(int type, char *name, int num_var)
{
    extern int static_variable_flag;
    struct lnode_var_def *p;

    p = (struct lnode_var_def *)xalloc(sizeof (struct lnode_var_def));
    p->type = type;
    p->line = current_line + L_VAR_DEF;
    p->name = make_shared_string(name);
    free(name);
    p->num_var = num_var;
    p->next = prog_status;
    p->is_static = static_variable_flag;
    prog_status = p;
    tot_var_def++;
}

static int tot_funcall;
struct lnode_funcall *alloc_lnode_funcall(int type, char *name,
    struct lnode *arg)
{
    struct lnode_funcall *p;

    p = (struct lnode_funcall *)xalloc(sizeof (struct lnode_funcall));
    p->type = type;
    p->line = current_line + L_FUNCALL;
    p->name = make_shared_string(name);
    free(name);
    p->expr = arg;
    tot_funcall++;
    return p;
}

/*
 * Statements in a block (list of statements) are stored with cons
 * node (lnode_2). This is very expensive, as one lnode_2 is required as
 * overhead for every statement. This function will take such a list
 * and move all statements in a block into one single memory vector, storing
 * them consecutively.
 */
int tot_lnode_block;
struct lnode_block *alloc_lnode_block(struct lnode *p)
{
    int size, num;
    char *block, *block2;
    struct lnode *l, *next;
    struct lnode_block *ret;

    if (p == 0)
	return 0;
    for (num = 0, size = 0, l=p; l; l = l->a2) {
	size += lnode_size[l->a1->line >> L_SHIFT];
	num++;
    }
    if (num == 1) {
	/* Only one statement. No need to make a block ! */
	struct lnode *stat;

	stat = p->a1;
	free_lnode(p, 1);
	return (struct lnode_block *)stat;
    }
    block = xalloc(sizeof (int) + size + 100);
    block2 = block;
    for (l=p; l; l = next) {
	int s;

	s = lnode_size[l->a1->line >> L_SHIFT];
	memcpy(block2, (char *)l->a1, s);
	free_lnode(l->a1, 0);	/* Node has been copied. */
	l->a1 = 0;
	block2 += s;
	next = l->a2;
	free_lnode(l, 1);
    }
    ret = (struct lnode_block *)xalloc(sizeof (struct lnode_block));
    ret->type = F_BLOCK;
    ret->line = current_line + L_BLOCK;
    ret->block = block;
    ret->num_nodes = num;
    tot_lnode_block++;
    return ret;
}

void print_lnode_status(int tot_before_this)
{
    int tot =
	tot_lnode_number * sizeof (struct lnode_number) +
	tot_lnode_name * sizeof (struct lnode_name) +
	tot_lnode_1 * sizeof (struct lnode_1) +
	tot_lnode_2 * sizeof (struct lnode_2) +
	tot_lnode_3 * sizeof (struct lnode_3) +
	tot_var_def * sizeof (struct lnode_var_def) +
	tot_funcall * sizeof (struct lnode_funcall) +
	tot_lnode_def * sizeof (struct lnode_def) +
	tot_lnode_block * sizeof (struct lnode_block) +
	tot_before_this;
    add_message("lnode:\n");
    add_message("   single:     %5d %6d\n", tot_lnode_single,
		tot_lnode_single * sizeof (struct lnode_single));
    add_message("   numbers:    %5d %6d\n", tot_lnode_number,
		tot_lnode_number * sizeof (struct lnode_number));
    add_message("   names:      %5d %6d\n", tot_lnode_name,
		tot_lnode_name * sizeof (struct lnode_name));
    add_message("   1 size expr:%5d %6d\n", tot_lnode_1,
		tot_lnode_1 * sizeof (struct lnode_1));
    add_message("   2 size expr:%5d %6d\n", tot_lnode_2,
		tot_lnode_2 * sizeof (struct lnode_2));
    add_message("   3 size expr:%5d %6d\n", tot_lnode_3,
		tot_lnode_3 * sizeof (struct lnode_3));
    add_message("   functions:  %5d %6d\n", tot_lnode_def,
		tot_lnode_def * sizeof (struct lnode_def));
    add_message("   glob vars:  %5d %6d\n", tot_var_def,
		tot_var_def * sizeof (struct lnode_var_def));
    add_message("   fun calls:  %5d %6d\n", tot_funcall,
		tot_funcall * sizeof (struct lnode_funcall));
    add_message("   blocks:     %5d %6d\n", tot_lnode_block,
		tot_lnode_block * sizeof (struct lnode_block));
    add_message("   Total bytes       %6d\n", tot);
}

int lnode_size[(L_MAX >> L_SHIFT) + 1];

/*
 * Setup fast access to the size of an lnode.
 */

void compute_lnode_size()
{
    lnode_size[L_SINGLE >> L_SHIFT] = sizeof (struct lnode_single);
    lnode_size[L_NUMBER >> L_SHIFT] = sizeof (struct lnode_number);
    lnode_size[L_NAME >> L_SHIFT] = sizeof (struct lnode_name);
    lnode_size[L_VARIABLE >> L_SHIFT] = sizeof (struct lnode_variable);
    lnode_size[L_1 >> L_SHIFT] = sizeof (struct lnode_1);
    lnode_size[L_2 >> L_SHIFT] = sizeof (struct lnode_2);
    lnode_size[L_3 >> L_SHIFT] = sizeof (struct lnode_3);
    lnode_size[L_DEF >> L_SHIFT] = sizeof (struct lnode_def);
    lnode_size[L_VARIABLE >> L_SHIFT] = sizeof (struct lnode_var_def);
    lnode_size[L_FUNCALL >> L_SHIFT] = sizeof (struct lnode_funcall);
    lnode_size[L_BLOCK >> L_SHIFT] = sizeof (struct lnode_block);
}

void free_lnode(struct lnode *p, int update_count)
{
    switch(p->line & L_MASK) {
    case L_SINGLE:
	if (update_count)
	    tot_lnode_single--;
	break;
    case L_NUMBER:
	((struct lnode_number *)p)->next = lnode_number_list;
	lnode_number_list = (struct lnode_number *)p;
	return;
    case L_NAME:
	((struct lnode_name *)p)->name = (char *)lnode_name_list;
	lnode_name_list = (struct lnode_name *)p;
	return;
    case L_VARIABLE:
	if (update_count)
	    tot_lnode_variable--;
	break;
    case L_1:
	((struct lnode_1 *)p)->expr = (struct lnode *)lnode_1_list;
	lnode_1_list = (struct lnode_1 *)p;
	return;
    case L_2:
	((struct lnode_2 *)p)->expr1 = (struct lnode *)lnode_2_list;
	lnode_2_list = (struct lnode_2 *)p;
	return;
    case L_3:
	if (update_count)
	    tot_lnode_3--;
	break;
    case L_DEF:
	if (update_count)
	    tot_lnode_def--;
	break;
    case L_VAR_DEF:
	if (update_count)
	    tot_var_def--;
	break;
    case L_FUNCALL:
	if (update_count)
	    tot_funcall--;
	break;
    case L_BLOCK:
	if (update_count)
	    tot_lnode_block--;
	break;
    default:
	fatal("Bad type in lnode_size(): 0x%x\n", p->line & L_MASK);
    }
    free((char *)p);
}

void add_prog_ref(struct lnode_def *p)
{
    p->num_ref++;
}

/*
 * The program is stored at one of two places. Either a tree pointed
 * to by ob->prog (first argument), or a consecutive area pointed
 * to by 'block'. These two different trees has to be free'd in different
 * ways. It is load_ob_from_swap() that loads a prog into the ob->block
 * instead of ob->prog. ob->block will also contain all variable definitions.
 */
int free_prog(struct lnode_def *p, char *block, int size)
{
    extern int total_num_prog_blocks, total_prog_block_size;
    p->num_ref--;
    if (p->num_ref > 0)
	return 0;
    if (block) {
	/* free_lnode_string((struct lnode *)block); */
	free(block);
	total_prog_block_size -= size;
	total_num_prog_blocks -= 1;
    } else {
	free_sub_part((struct lnode *)p, 1);
    }
    return 1;
}

/*
 * Free the space of one node, and all linkes from it.
 * When a L_BLOCK is freed, don't call free_lnode() with the nodes
 * in the top of the block, because these are allocated in one big
 * chunk.
 */
void free_sub_part(struct lnode *p, int do_free)
{
    extern int tot_alloc_strings;
    if (p == 0)
	return;
    switch(p->line & L_MASK) {
    case L_SINGLE:
	break;
    case L_NUMBER:
	break;
    case L_NAME:
	free_string(((struct lnode_name *)p)->name);
	break;
    case L_VARIABLE:
	break;
    case L_1:
	free_sub_part(((struct lnode_1 *)p)->expr, 1);
	((struct lnode_1 *)p)->expr = 0;
	break;
    case L_2:
	free_sub_part(((struct lnode_2 *)p)->expr1, 1);
	((struct lnode_2 *)p)->expr1 = 0;
	free_sub_part(((struct lnode_2 *)p)->expr2, 1);
	((struct lnode_2 *)p)->expr2 = 0;
	break;
    case L_3:
	free_sub_part(((struct lnode_3 *)p)->expr1, 1);
	((struct lnode_3 *)p)->expr1 = 0;
	free_sub_part(((struct lnode_3 *)p)->expr2, 1);
	((struct lnode_3 *)p)->expr2 = 0;
	free_sub_part(((struct lnode_3 *)p)->expr3, 1);
	((struct lnode_3 *)p)->expr3 = 0;
	break;
    case L_DEF:
    {
	struct lnode_def *dp = (struct lnode_def *)p;
	free_string(dp->name);
	dp->name = 0;
	free_sub_part(dp->block, 1);
	dp->block = 0;
	if (dp->next) {
	    free_sub_part((struct lnode *)dp->next, 1);
	    dp->next = 0;
	}
	break;
    }
    case L_VAR_DEF:
    {
	struct lnode_var_def *lp;
	lp = (struct lnode_var_def *)p;
	if (lp->next)
	    free_sub_part((struct lnode *)lp->next, 1);
	free_string(lp->name);
	lp->name = 0;
	break;
    }
    case L_FUNCALL:
	free_string(((struct lnode_funcall *)p)->name);
	((struct lnode_funcall *)p)->name = 0;
	free_sub_part(((struct lnode_funcall *)p)->expr, 1);
	((struct lnode_funcall *)p)->expr = 0;
	break;
    case L_BLOCK:
    {
	struct lnode_block *lb = (struct lnode_block *)p;
	char *block = lb->block;
	int i;
	
	for (i=0; i < lb->num_nodes; i++) {
	    free_sub_part((struct lnode *)block, 0);
	    block += lnode_size[((struct lnode *)block)->line >> L_SHIFT];
	}
	free(lb->block);
	break;
    }
    default:
	fatal("Bad type in free_sub_part(): 0x%x\n", p->line & L_MASK);
    }
    if (do_free)
	free_lnode(p, 1);
}

/*
 * Free shared strings used by this lnode.
 * This is only done for programs that has been swapped out, and
 * thus are allocated in one big block. Then, only strings has to
 * be manually freed.
 */
void free_lnode_string(struct lnode *p)
{
    if (p == 0)
	return;
    switch(p->line & L_MASK) {
    case L_CONSTANT:
	break;
    case L_SINGLE:
	break;
    case L_NUMBER:
	break;
    case L_NAME:
	free_string(((struct lnode_name *)p)->name);
	break;
    case L_VARIABLE:
	break;
    case L_1:
	free_lnode_string(((struct lnode_1 *)p)->expr);
	break;
    case L_2:
	free_lnode_string(((struct lnode_2 *)p)->expr1);
	free_lnode_string(((struct lnode_2 *)p)->expr2);
	break;
    case L_3:
	free_lnode_string(((struct lnode_3 *)p)->expr1);
	free_lnode_string(((struct lnode_3 *)p)->expr2);
	free_lnode_string(((struct lnode_3 *)p)->expr3);
	break;
    case L_DEF:
    {
	struct lnode_def *dp = (struct lnode_def *)p;
	free_string(dp->name);
	free_lnode_string(dp->block);
	if (dp->next) {
	    free_lnode_string((struct lnode *)dp->next);
	}
	break;
    }
    case L_VAR_DEF:
    {
	struct lnode_var_def *lp;
	lp = (struct lnode_var_def *)p;
	if (lp->next)
	    free_lnode_string((struct lnode *)lp->next);
	free_string(lp->name);
	break;
    }
    case L_FUNCALL:
	free_string(((struct lnode_funcall *)p)->name);
	free_lnode_string(((struct lnode_funcall *)p)->expr);
	break;
    case L_BLOCK:
    {
	struct lnode_block *lb = (struct lnode_block *)p;
	char *block = lb->block;
	int i;
	
	for (i=0; i < lb->num_nodes; i++) {
	    free_lnode_string((struct lnode *)block);
	    block += lnode_size[((struct lnode *)block)->line >> L_SHIFT];
	}
	break;
    }
    default:
	fatal("Bad type in free_lnode_string(): 0x%x\n", p->line & L_MASK);
    }
}

#define NUM_LNODE_NUMBER_CONST		20
static struct lnode_number lnode_number_const[NUM_LNODE_NUMBER_CONST];
static struct lnode_number lnode_number_const_id[NUM_LNODE_NUMBER_CONST];
static struct lnode_number lnode_number_const_loc[NUM_LNODE_NUMBER_CONST];
static int num_number_match;

static int lnode_func_list[] =
{
    F_ENABLE_COMMANDS, F_THIS_OBJECT, F_THIS_PLAYER, F_TIME, F_USERS,
    F_RETURN,
};
#define NUM_FUNCS	(sizeof lnode_func_list / sizeof lnode_func_list[0])
static struct lnode_1 funcalls[NUM_FUNCS];
static int num_funcall_match;

void init_constant_lnodes(void) {
    int i;

    for (i=0; (size_t)i < NUM_FUNCS; i++) {
	funcalls[i].line = L_CONSTANT;
	funcalls[i].type = lnode_func_list[i];
	funcalls[i].expr = 0;
    }
    for (i=0; i < NUM_LNODE_NUMBER_CONST; i++) {
	lnode_number_const[i].line = L_CONSTANT;
	lnode_number_const[i].type = F_NUMBER;
	lnode_number_const[i].number = i;
	lnode_number_const_id[i].line = L_CONSTANT;
	lnode_number_const_id[i].type = F_IDENTIFIER;
	lnode_number_const_id[i].number = i;
	lnode_number_const_loc[i].line = L_CONSTANT;
	lnode_number_const_loc[i].type = F_LOCAL_NAME;
	lnode_number_const_loc[i].number = i;
    }
}

struct lnode_single lnode_const0 = { F_CONST0, L_CONSTANT };
struct lnode_single lnode_const1 = { F_CONST1, L_CONSTANT };
int num_lnode_single;

int match_and_replace_lnode(struct lnode *match, struct lnode **destp)
{
    struct lnode_number *p = (struct lnode_number *)match;
    struct lnode_1 *p1 = (struct lnode_1 *)match;

    if (match->type == F_CONST0) {
	*destp = (struct lnode *)&lnode_const0;
	num_lnode_single++;
	return 1;
    }
    if (match->type == F_CONST1) {
	*destp = (struct lnode *)&lnode_const1;
	num_lnode_single++;
	return 1;
    }
    if ((p1->line & L_MASK) == L_1 && p1->expr == 0) {
	int i;

	for (i=0; (size_t)i < NUM_FUNCS; i++) {
	    if (p1->type != lnode_func_list[i])
		continue;
	    *destp = (struct lnode *)&funcalls[i];
	    num_funcall_match++;
	    return 1;
	}
	return 0;
    }
    if ((p->line & L_MASK) == L_NUMBER && p->type == F_NUMBER) {
	if (p->number < 0 || p->number >= NUM_LNODE_NUMBER_CONST)
	    return 0;
	*destp = (struct lnode *)&lnode_number_const[p->number];
	num_number_match++;
	return 1;
    }
    if ((p->line & L_MASK) == L_NUMBER && p->type == F_IDENTIFIER) {
	if (p->number < 0 || p->number >= NUM_LNODE_NUMBER_CONST)
	    return 0;
	*destp = (struct lnode *)&lnode_number_const_id[p->number];
	num_number_match++;
	return 1;
    }
    if ((p->line & L_MASK) == L_NUMBER && p->type == F_LOCAL_NAME) {
	if (p->number < 0 || p->number >= NUM_LNODE_NUMBER_CONST)
	    return 0;
	*destp = (struct lnode *)&lnode_number_const_loc[p->number];
	num_number_match++;
	return 1;
    }
    return 0;
}

void status_lnodes_matched(void) {
    add_message("Saved lnode_numbers: %6d (%7d bytes)\n", num_number_match,
		num_number_match * sizeof (struct lnode_number) -
		sizeof lnode_number_const_loc -
		sizeof lnode_number_const_id -
		sizeof lnode_number_const);
    add_message("Saved lnode_1      : %6d (%7d bytes)\n", num_funcall_match,
		sizeof (struct lnode_1) * (num_funcall_match - NUM_FUNCS));
    add_message("Saved lnode_single : %6d (%7d bytes)\n", num_lnode_single,
		sizeof (struct lnode_single) * (num_lnode_single - 2));
}
