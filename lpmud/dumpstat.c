#include <stdio.h>
#include <string.h>
#include "lint.h"
#include "interpret.h"
#include "object.h"
#include "lnode.h"
/*
 * Write statistics about objects on file.
 */

extern struct object *obj_list;

static int svalue_size(struct svalue *v) {
    int i, total;

    switch(v->type) {
    case T_OBJECT:
    case T_NUMBER:
	return 0;
    case T_STRING:
	return strlen(v->u.string) + 4; /* Includes some malloc overhead. */
    case T_POINTER:
	for (i=0, total = 0; i < v->u.vec->size; i++) {
	    total += svalue_size(&v->u.vec->item[i]) + sizeof (struct svalue);
	}
	return total;
    default:
	fatal("Illegal type: %d\n", v->type);
	return 0;
    }
}

static int data_size(struct object *ob) {
    int total = 0, i;

    for (i = 0; i < ob->num_variables; i++)
	total += svalue_size(&ob->variables[i]) + sizeof (struct svalue);
    return total;
}

void dumpstat() {
    FILE *f;
    struct object *ob;

    f = fopen("OBJ_DUMP", "w");
    if (f == 0)
	return;
    add_message("Dumping to OBJ_DUMP ...");
    for (ob = obj_list; ob; ob = ob->next_all) {
	int tmp;
	if (ob->swapped || ob->prog->num_ref == 1 || !ob->cloned)
	    tmp = ob->swap_size;
	else
	    tmp = 0;
	fprintf(f, "%-20s %5lu ref %2d %s %s\n", ob->name,
		(unsigned long)(tmp + data_size(ob) + sizeof (struct object)), ob->ref,
		ob->enable_heart_beat ? "HB" : "  ",
		ob->super ? ob->super->name : "--");
    }
    add_message("done.\n");
    fclose(f);
}
