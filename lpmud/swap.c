#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "lint.h"
#include "interpret.h"
#include "object.h"
#include "lnode.h"
#include "config.h"

/*
 * Swap out programs from objects.
 */
#define SWAP_SIZE	50000	/* Big enough for most objects. */

extern char *xalloc PROT((size_t)), *string_copy PROT((char *));
static void move_prog_to_swap_area PROT((struct lnode *, char *));
void load_prog_swap_area PROT((char *, char *));
static void copy_sub_node PROT((struct lnode *, struct lnode **));

static char *swap_area;
static int swap_max_size, swap_size;

int num_swapped;
int total_bytes_swapped;

FILE *swap_file;		/* The swap file is opened once */

int total_num_prog_blocks, total_prog_block_size;
/*
 * Used to determine if a pointer is pointing inside a lnode swap
 * block.
 */
static int current_buffer_size;
#define OFFSET_AS_PTR(type, off) ((type *)(uintptr_t)(off))
#define STORED_OFFSET(p) ((uintptr_t)(p))
#define IS_STORED_OFFSET(p) ((p) && STORED_OFFSET(p) < (uintptr_t)current_buffer_size)

/*
 * Swap out an object. This is done by copying all struct lnodes into
 * one contiguous area, and replacing all pointers with relative offsets.
 */
int swap(struct object *ob)
{
    struct lnode_2 swap_node;

    if (ob->swap_num == -1 && ob->block != 0 && !ob->cloned) {
	fprintf(stderr, "FATAL: swap(%s) swap_num %d block %p\n",
		ob->name, ob->swap_num, (void *)ob->block);
	return 0;
    }
    if (swap_file == 0) {
	char file_name[100], host[50];
	gethostname(host, sizeof host);
	sprintf(file_name, "%s.%s", SWAP_FILE, host);
	swap_file = fopen(file_name, "w+");
	if (swap_file == 0)
	    return 0;
    }
    if (!ob->prog)
    {
	fprintf(stderr, "warning:no program in object %s, don't swap it\n",
		ob->name);
	/* It`s no good freeing a NULL pointer */
	return 0;
    }
    if (NUM_RESET_TO_SWAP == 0)
	return 0;
    if (ob->enable_heart_beat || ob->cloned)
	return 0;
    if (ob->prog->num_ref > 1 || ob->inherited || ob->interactive)
	return 0;
    if (ob->swap_num >= 0) {
	if (!ob->block)
	    fatal("Object swapped but prog not in block.\n");
	if (!free_prog(ob->prog, ob->block, ob->swap_size))
	    fatal("Failed to release prog block when swap.\n");
	ob->prog = 0;
	ob->status = 0;
	ob->swapped = 1;
	ob->heart_beat = 0;	/* This pointer has to be set up again */
	num_swapped++;
	total_bytes_swapped += ob->swap_size;
	return 1;
    }
    swap_area = xalloc(SWAP_SIZE);
    swap_size = 0;
    swap_max_size = SWAP_SIZE;
    swap_node.line = L_2;
    swap_node.expr1 = (struct lnode *)ob->prog;
    swap_node.expr2 = (struct lnode *)ob->status;
    move_prog_to_swap_area((struct lnode *)&swap_node, 0);
    if (fseek(swap_file, 0, 2) == -1) {
	free(swap_area);
	return 0;
    }
    ob->swap_num = ftell(swap_file);
    if (fwrite(swap_area, 1, swap_size, swap_file) != (size_t)swap_size) {
	debug_message("I/O error in swap.\n");
	free(swap_area);
	return 0;
    }
    total_bytes_swapped += swap_size;
    num_swapped++;
    free(swap_area);
    if (ob->block)
	fprintf(stderr, "FATAL: Object prog already in block: %s, %p.\n",
		ob->name, (void *)ob->block);
    if (!free_prog(ob->prog, 0, 0))
	fatal("Failed to release prog tree when first swap.\n");
    if (ob->status)
	free_sub_part((struct lnode *)ob->status, 1);
    ob->prog = 0;
    ob->status = 0;
    ob->swapped = 1;
    ob->heart_beat = 0;		/* This pointer has to be set up again */
    ob->swap_size = swap_size;
    return 1;
}

static void ass_size(int s)
{
    char *temp;
    int tempsize;

    while(s + swap_size > swap_max_size) {
	tempsize = swap_max_size;
	swap_max_size += SWAP_SIZE;
	temp = xalloc (swap_max_size);
	if (temp == 0) {
	    fatal("swap realloc %d\n", swap_max_size);
	    return;
	}
	if (swap_area != 0 && tempsize > 0)
	    memcpy(temp, swap_area, tempsize);
	free(swap_area);
	swap_area = temp;
    }
}

#define even(s) ((s+3)&~3)

struct lnode_block *lb_glob;
struct lnode *p_glob;

/*
 * Move lnode 'p' to the swap area, and convert all pointers to relative
 * pointers. If 'dest' is non-zero, it is the offset into the swaparea
 * where 'p' is supposed to be copied. If 'dest', then space is supposed
 * to have been reserved.
 */
static void move_prog_to_swap_area(struct lnode *p, char *dest)
{
    unsigned int s;
    int beg = swap_size;

    if (dest)
	beg = (int)(dest - swap_area);
    switch(p->line & L_MASK) {
    case L_SINGLE:
	s = sizeof(struct lnode_single);
	ass_size(s);
	memcpy(swap_area + beg, (char *)p, s);
	if (!dest)
	    swap_size += s;
	break;
    case L_NUMBER:
	s = sizeof(struct lnode_number);
	ass_size(s);
	memcpy(swap_area + beg, (char *)p, s);
	if (!dest)
	    swap_size += s;
	break;
    case L_NAME:
	s = sizeof(struct lnode_name);
	ass_size(s);
	memcpy(swap_area + beg, (char *)p, s);
	if (!dest)
	    swap_size += s;
#ifdef SWAP_STRINGS
	((struct lnode_name *)(swap_area+beg))->name =
	    OFFSET_AS_PTR(char, swap_size);
	s = even(strlen(((struct lnode_name *)p)->name) + 1);
	ass_size(s);
	strcpy(swap_area + swap_size, ((struct lnode_name *)p)->name);
	swap_size += s;
#else
	make_shared_string(((struct lnode_name *)p)->name);
#endif
	break;
    case L_1:
    {
	struct lnode_1 *l = (struct lnode_1 *)p;

	s = sizeof(struct lnode_1);
	ass_size(s);
	memcpy(swap_area + beg, (char *)p, s);
	if (!dest)
	    swap_size += s;
	copy_sub_node(l->expr, &((struct lnode_1 *)(swap_area+beg))->expr);
	break;
    }
    case L_2:
    {
	struct lnode_2 *l = (struct lnode_2 *)p;

	s = sizeof(struct lnode_2);
	ass_size(s);
	memcpy(swap_area + beg, (char *)p, s);
	if (!dest)
	    swap_size += s;
	copy_sub_node(l->expr1, &((struct lnode_2 *)(swap_area+beg))->expr1);
	copy_sub_node(l->expr2, &((struct lnode_2 *)(swap_area+beg))->expr2);
	break;
    }
    case L_3:
    {
	struct lnode_3 *l = (struct lnode_3 *)p;

	s = sizeof(struct lnode_3);
	ass_size(s);
	memcpy(swap_area + beg, (char *)p, s);
	if (!dest)
	    swap_size += s;
	copy_sub_node(l->expr1, &((struct lnode_3 *)(swap_area+beg))->expr1);
	copy_sub_node(l->expr2, &((struct lnode_3 *)(swap_area+beg))->expr2);
	copy_sub_node(l->expr3, &((struct lnode_3 *)(swap_area+beg))->expr3);
	break;
    }
    case L_DEF:
    {
	struct lnode_def *dp;
	for (dp = (struct lnode_def *)p; dp; dp = dp->next) {
	    beg = swap_size;	/* Moved forward for every function. */
	    s = sizeof(struct lnode_def);
	    ass_size(s);
	    memcpy(swap_area + beg, (char *)dp, s);
	    swap_size += s;
#ifdef SWAP_STRINGS
	    ((struct lnode_def *)(swap_area+beg))->name =
		OFFSET_AS_PTR(char, swap_size);
	    s = even(strlen(dp->name) + 1);
	    ass_size(s);
	    strcpy(swap_area + swap_size, dp->name);
	    swap_size += s;
#else
	    make_shared_string(dp->name);
#endif
	    /* The block can be empty ! */
	    copy_sub_node(dp->block,
			  &((struct lnode_def *)(swap_area+beg))->block);
	    if (dp->next) {
		((struct lnode_def *)(swap_area+beg))->next =
		    OFFSET_AS_PTR(struct lnode_def, swap_size);
	    } else
		((struct lnode_def *)(swap_area+beg))->next = 0;
	}
	break;
    }
    case L_FUNCALL:
    {
	struct lnode_funcall *l = (struct lnode_funcall *)p;

	s = sizeof (struct lnode_funcall);
	ass_size(s);
	memcpy(swap_area + beg, (char *)p, s);
	if (!dest)
	    swap_size += s;
#ifdef SWAP_STRINGS
	((struct lnode_funcall *)(swap_area+beg))->name =
	    OFFSET_AS_PTR(char, swap_size);
	s = even(strlen(((struct lnode_funcall *)p)->name) + 1);
	ass_size(s);
	strcpy(swap_area + swap_size, ((struct lnode_funcall *)p)->name);
	swap_size += s;
#else
	make_shared_string(((struct lnode_funcall *)p)->name);
#endif
	copy_sub_node(l->expr,
		      &((struct lnode_funcall *)(swap_area+beg))->expr);
	break;
    }
    case L_BLOCK:
    {
	struct lnode_block *lb = (struct lnode_block *)p;
	char *block;
	int i, block2;
	
	s = sizeof (struct lnode_block);
	ass_size(s);
	memcpy(swap_area + beg, (char *)p, s);
	if (!dest)
	    swap_size += s;
	if (lb->num_nodes) {
	    ((struct lnode_block *)(swap_area+beg))->block =
		OFFSET_AS_PTR(char, swap_size);
	} else
	    ((struct lnode_block *)(swap_area+beg))->block = 0;
	block2 = swap_size;
	/*
	 * Reserve size for all lnodes in this block.
	 */
	for (i=0, block = lb->block; i < lb->num_nodes; i++) {
	    s = lnode_size[((struct lnode *)block)->line >> L_SHIFT];
	    ass_size(s);
	    swap_size += s;
	    block += s;
	}
	/*
	 * Now copy the lnodes in this block.
	 */
	for (i=0, block = lb->block; i < lb->num_nodes; i++) {
	    move_prog_to_swap_area((struct lnode *)block,
				   swap_area + block2);
	    s = lnode_size[((struct lnode *)block)->line >> L_SHIFT];
	    block += s;
	    block2 += s;
	}
	break;
    }
    case L_VAR_DEF:
    {
	struct lnode_var_def *vd;

	for (vd = (struct lnode_var_def *)p; vd; vd = vd->next) {
	    beg = swap_size;
	    s = sizeof (struct lnode_var_def);
	    ass_size(s);
	    memcpy(swap_area + beg, (char *)vd, s);
	    swap_size += s;
#ifdef SWAP_STRINGS
	    ((struct lnode_var_def *)(swap_area+beg))->name =
		OFFSET_AS_PTR(char, swap_size);
	    s = even(strlen(vd->name) + 1);
	    ass_size(s);
	    strcpy(swap_area + swap_size, vd->name);
	    swap_size += s;
#else
	    make_shared_string(vd->name);
#endif
	    if (vd->next) {
		((struct lnode_var_def *)(swap_area+beg))->next =
		    OFFSET_AS_PTR(struct lnode_var_def, swap_size);
	    } else
		((struct lnode_var_def *)(swap_area+beg))->next = 0;
	}
	break;
    }
    case L_VARIABLE:
    default:
	fatal("Bad type in free_sub_part(): 0x%x\n", p->line);
    }
}
static struct object *tmp_ob;

void load_ob_from_swap(struct object *ob)
{
    char *buffer;
    struct lnode_def *pr;
    struct lnode_2 *swap_node;
    tmp_ob = ob;
    if (ob->swap_num == -1)
	fatal("Loading not swapped object.\n");
    total_prog_block_size += ob->swap_size;
    total_num_prog_blocks += 1;
    buffer = xalloc(ob->swap_size);
    current_buffer_size = ob->swap_size;
    if (fseek(swap_file, ob->swap_num, 0) == -1)
	fatal("Couldn't seek the swap file, errno %d, offset %d.\n",
	      errno, ob->swap_num);
    if (fread(buffer, 1, ob->swap_size, swap_file) != ob->swap_size)
	fatal("Couldn't read the swap file.\n");
    load_prog_swap_area(buffer, buffer);
    swap_node = (struct lnode_2 *)buffer;
    ob->block = buffer;
    ob->prog = (struct lnode_def *)swap_node->expr1;
    ob->status = (struct lnode_var_def *)swap_node->expr2;
    /* The reference count will already be 1 ! */
    ob->swapped = 0;
    total_bytes_swapped -= ob->swap_size;
    num_swapped--;
    /*
     * Now we have to restore the heart_beat pointer.
     */
    for (pr = ob->prog; pr; pr = pr->next) {
	if (strcmp(pr->name, "heart_beat") == 0) {
	    ob->heart_beat = (struct lnode *)pr;
	    break;
	}
    }
    if (!ob->heart_beat && ob->inherit)
	ob->heart_beat = ob->inherit->heart_beat;
}

char *base_glob, *s_glob;
void load_prog_swap_area(char *base, char *s)
{
    struct lnode *p = (struct lnode *)s;

    base_glob = base;
    s_glob = s;
    p_glob = p;
    if (s < base)
	fatal("base < s\n");
    switch(p->line & L_MASK) {
    case L_CONSTANT:
	break;
    case L_SINGLE:
	break;
    case L_NUMBER:
	break;
    case L_NAME:
    {
#ifdef SWAP_STRINGS
	struct lnode_name *pn = (struct lnode_name *)p;
	pn->name = base + STORED_OFFSET(pn->name);
#endif
	break;
    }
    case L_1:
    {
	struct lnode_1 *p1 = (struct lnode_1 *)p;
	if (IS_STORED_OFFSET(p1->expr)) {
	    p1->expr = (struct lnode *)(base + STORED_OFFSET(p1->expr));
	    load_prog_swap_area(base, (char *)p1->expr);
	}
	break;
    }
    case L_2:
    {
	struct lnode_2 *p2 = (struct lnode_2 *)p;
	if (IS_STORED_OFFSET(p2->expr1)) {
	    p2->expr1 = (struct lnode *)(base + STORED_OFFSET(p2->expr1));
	    load_prog_swap_area(base, (char *)p2->expr1);
	}
	if (IS_STORED_OFFSET(p2->expr2)) {
	    p2->expr2 = (struct lnode *)(base + STORED_OFFSET(p2->expr2));
	    load_prog_swap_area(base, (char *)p2->expr2);
	}
	break;
    }
    case L_3:
    {
	struct lnode_3 *p3 = (struct lnode_3 *)p;
	if (IS_STORED_OFFSET(p3->expr1)) {
	    p3->expr1 = (struct lnode *)(base + STORED_OFFSET(p3->expr1));
	    load_prog_swap_area(base, (char *)p3->expr1);
	}
	if (IS_STORED_OFFSET(p3->expr2)) {
	    p3->expr2 = (struct lnode *)(base + STORED_OFFSET(p3->expr2));
	    load_prog_swap_area(base, (char *)p3->expr2);
	}
	if (IS_STORED_OFFSET(p3->expr3)) {
	    p3->expr3 = (struct lnode *)(base + STORED_OFFSET(p3->expr3));
	    load_prog_swap_area(base, (char *)p3->expr3);
	}
	break;
    }
    case L_DEF:
    {
	/*
	 * Unwind all function definitions here, so that recursive call
	 * of load_prog_swap_area() of them not is needed.
	 */
	struct lnode_def *dp;
	for (dp = (struct lnode_def *)p; dp; dp = dp->next) {
	    if (IS_STORED_OFFSET(dp->block)) {
		dp->block = (struct lnode *)(base + STORED_OFFSET(dp->block));
		load_prog_swap_area(base, (char *)dp->block);
	    }
#ifdef SWAP_STRINGS
	    dp->name = base + STORED_OFFSET(dp->name);
#endif
	    if (dp->next)
		dp->next = (struct lnode_def *)(base + STORED_OFFSET(dp->next));
	}
	break;
    }
    case L_FUNCALL:
    {
	struct lnode_funcall *pf = (struct lnode_funcall *)p;
	if (IS_STORED_OFFSET(pf->expr)) {
	    pf->expr = (struct lnode *)(base + STORED_OFFSET(pf->expr));
	    load_prog_swap_area(base, (char *)pf->expr);
	}
#ifdef SWAP_STRINGS
	pf->name = base + STORED_OFFSET(pf->name);
#endif
	break;
    }
    case L_BLOCK:
    {
	int size, num;
	char *block_start = 0;
	struct lnode *l;
	struct lnode_block *lb = (struct lnode_block *)p;

	if (lb->block) {
	    block_start = base + STORED_OFFSET(lb->block);
	    lb->block = block_start;
	}
	if (block_start == 0 || lb->num_nodes <= 0)
	    break;
	for (num = 0, size = 0; num < lb->num_nodes; num++) {
	    l = (struct lnode *)(block_start + size);
	    load_prog_swap_area(base, (char *)l);
	    size += lnode_size[l->line >> L_SHIFT];
	}
	break;
    }
    case L_VAR_DEF:
    {
	/*
	 * Unwind all variable definitions here, so that recursive call
	 * of load_prog_swap_area() of them not is needed.
	 */
	struct lnode_var_def *vd;
	for (vd = (struct lnode_var_def *)p; vd; vd = vd->next) {
#ifdef SWAP_STRINGS
	    vd->name = base + STORED_OFFSET(vd->name);
#endif
	    if (vd->next)
		vd->next = (struct lnode_var_def *)(base + STORED_OFFSET(vd->next));
	}
	break;
    }
    case L_VARIABLE:
    default:
	fprintf(stderr, "swap_area = %p, base = %p\n", (void *)swap_area, (void *)base);
	fatal("Bad type in load_prog_swap_area 0x%x\n", p->line);
    }
}

void remove_swap_file(struct object *ob)
{
    /* Haven't implemented this yet :-( */
    (void)ob;
}

/*
 * Warning, this function is not called every time.
 */
static void copy_sub_node(struct lnode *from, struct lnode **top)
{
    if (from) {
	if (!match_and_replace_lnode(from, top)) {
	    *top = OFFSET_AS_PTR(struct lnode, swap_size);
	    move_prog_to_swap_area(from, 0);
	}
    } else {
	*top = 0;
    }
}
