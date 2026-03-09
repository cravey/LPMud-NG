#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include "lint.h"
#include "interpret.h"
#include "object.h"
#include "lnode.h"
#include "sent.h"
#include "config.h"
#include "wiz_list.h"

extern int d_flag;

extern char *xalloc PROT((size_t)), *string_copy PROT((char *));

void remove_swap_file PROT((struct object *));

extern struct object *previous_ob;

int tot_alloc_object;

/*
 * Replace newlines in a string with a carriage return, to make the string
 * writeable on one line.
 */

static void replace_newline(char *str)
{
    for (; *str; str++) {
	if (str[0] == '\n')
	    str[0] = '\r';
    }
}

/*
 * Replace carriage return in a string with newlines.
 */

static void restore_newline(char *str)
{
    for (; *str; str++) {
	if (str[0] == '\r')
	    str[0] = '\n';
    }
}


/*
 * Save an object to a file. Two kinds of saves are allowed.
 * First: standard objects from /room and /obj can save anywhere. These are
 *        "trusted".
 * Second: A wizard defined object can only save at the home directory
 *         of that wizard.
 */
void save_object(struct object *ob, char *file)
{
    char *name, tmp_name[40];
    int len;
    size_t file_len, wiz_name_len;
    FILE *f;
    struct lnode_var_def *p;
    int failed = 0;

    if (ob->wl) {
	wiz_name_len = strlen(ob->wl->name);
	file_len = strlen(file);
	if (strncmp(file, "players/", 8) != 0 ||
	    file_len < (size_t)(8 + wiz_name_len) ||
	    strncmp(file+8, ob->wl->name, wiz_name_len) != 0 ||
	    (file_len > (size_t)(8 + wiz_name_len) &&
	     file[8 + wiz_name_len] != '/') ||
	    strchr(file, '.')) {
	    error("Illegal save file name %s\n", file);
	}
    } else if (strncmp(current_object->name, "obj/", 4) != 0 &&
	strncmp(current_object->name, "room/", 5) != 0) {
	error("Illegal use of save_object()\n");
    }
    len = strlen(file);
    name = xalloc(len + 3);
    (void)strcpy(name, file);
    (void)strcat(name, ".o");
    sprintf(tmp_name, "tmp_file_name_%d", getpid());
    f = fopen(tmp_name, "w");
    if (f == 0) {
	fatal("Could not open %s for a save.\n", tmp_name);
	abort();
    }
    for (p = ob->status; p; p = p->next) {
	struct svalue *v = &ob->variables[p->num_var];
	char *new_string;

	if (p->is_static)
	    continue;
	if (v->type == T_NUMBER) {
	    if (fprintf(f, "%s %d\n", p->name, v->u.number) == EOF)
		failed = 1;
	} else if (v->type == T_STRING) {
	    new_string = string_copy(v->u.string);
	    replace_newline(new_string);
	    if (fprintf(f, "%s \"%s\"\n", p->name, new_string) == EOF)
		failed = 1;
	    free(new_string);
	}
    }
    (void)unlink(name);
    if (link(tmp_name, name) == -1) {
	perror(name);
	printf("Failed to link %s to %s\n", tmp_name, name);
	add_message("Failed to save object !\n");
    }
    (void)fclose(f);
    unlink(tmp_name);
    free(name);
    if (failed)
	add_message("Failed to save to file. Disk could be full.\n");
}

int restore_object(struct object *ob, char *file)
{
    char *name = 0, var[100], *val = 0, *buff = 0, *space;
    int len;
    FILE *f = 0;
    struct lnode_var_def *p;
    struct object *save = current_object;
    struct stat st;
    int ok = 0;

    len = strlen(file);
    name = xalloc(len + 3);
    (void)strcpy(name, file);
    if (len >= 2 && name[len-2] == '.' && name[len-1] == 'c')
	name[len-1] = 'o';
    else
	(void)strcat(name, ".o");
    f = fopen(name, "r");
    if (f == 0)
	goto done;
    if (fstat(fileno(f), &st) == -1) {
	perror(name);
	goto done;
    }
    if (st.st_size == 0)
	goto done;
    val = xalloc(st.st_size + 1);
    buff = xalloc(st.st_size + 1);
    current_object = ob;
    while(1) {
	struct svalue *v;

	if (fgets(buff, st.st_size + 1, f) == 0)
	    break;
	/* Remember that we have a newline at end of buff ! */
	space = strchr(buff, ' ');
	if (space == 0)
	    error("Illegal format when restore %s.\n", name);
	(void)strncpy(var, buff, space - buff);
	var[space - buff] = '\0';
	(void)strcpy(val, space+1);
	p = find_status(var, 0);
	if (p == 0 || p->is_static)
	    continue;
	v = &ob->variables[p->num_var];
	if (val[0] == '"') {
	    val[strlen(val) - 2] = '\0';	/* Strip trailing "\n */
	    restore_newline(val+1);
	    assign_svalue(v, make_string(val+1));
	    continue;
	}
	assign_svalue(v, make_number(atoi(val)));
    }
    ok = 1;
done:
    current_object = save;
    if (d_flag && ok)
	debug_message("Object %s restored from %s.\n", ob->name,
		      name ? name : "<null>");
    if (name)
	free(name);
    if (buff)
	free(buff);
    if (val)
	free(val);
    if (f)
	(void)fclose(f);
    return ok;
}

#if 0
/* This is replaced by hashed names for living objects ! */
/*
 * Search for a living object which answers to the id NAME.
 * If there are many living objects, this algorithm can become quite
 * heavy. It should be hashed in the future !
 */

struct object *find_living_object(name)
    char *name;
{
    struct object *ob;
    struct value *ret;
    struct value thing;

    for (ob = obj_list; ob; ob = ob->next_all) {
	if (!ob->enable_commands || ob->super == 0)
	    continue;
	thing.type = T_STRING,
	thing.u.string = name;
	ret = apply("id", ob, &thing);
	/* Did the object self destruct ? (unlikely, but possible) */
	if (ob->destructed)
	    return 0;
	if (ret == 0 || (ret->type == T_NUMBER && ret->u.number == 0))
	    continue;
	return ob;
    }
    return 0;
}
#endif /* 0 */

void tell_npc(struct object *ob, char *str)
{
    struct value thing;

    thing.type = T_STRING;
    thing.u.string = str;
    (void)apply("catch_tell", ob, &thing);
}

/*
 * Send a message to an object.
 * If it is an interactive object, it will go to his
 * screen. Otherwise, it will go to a local function
 * catch_tell() in that object. This enables communications
 * between players and NPC's, and between other NPC's.
 */
void tell_object(struct object *ob, char *str)
{
    struct object *save_command_giver;

    if (ob->interactive) {
	save_command_giver = command_giver;
	command_giver = ob;
	add_message("%s", str);
	command_giver = save_command_giver;
	return;
    }
    tell_npc(ob, str);
}

void free_object(struct object *ob, char *from)
{
    struct sentence *s;
    void check_ob_ref PROT((struct object *, char *));

    ob->ref--;
    if (d_flag)
	debug_message("Subtr ref to ob %s: %d (%s)\n", ob->name,
		      ob->ref, from);
    check_ob_ref(ob, from);
    if (ob->ref > 0)
	return;
    if (!ob->destructed) {
	/* This is fatal, and should never happen. */
	fprintf(stderr,
	    "FATAL: Object %p %s ref count 0, but not destructed (from %s).\n",
	    (void *)ob, ob->name, from);
	if (current_object)
	    fprintf(stderr, "Current object: %s\n", ob->name);
	ob->ref = 1;	/* Not good, but better than crash the game ! */
	return;
    }
    ob->reset = 0;
    if (ob->interactive)
	fatal("Tried to free an interactive object.\n");
    /*
     * If the program is freed, then we can also free the variable
     * declarations.
     */
    if (ob->prog) {
	if (free_prog((struct lnode_def *)ob->prog, ob->block,
		      ob->swap_size)) {
	    if (ob->status && !ob->block)
		free_sub_part((struct lnode *)ob->status, 1);
	    ob->status = 0;
	    ob->prog = 0;
	    ob->block = 0;
	}
    }
    if (ob->swap_num != -1)
	remove_swap_file(ob);
    for (s = ob->sent; s;) {
	struct sentence *next;
	next = s->next;
	free_sentence(s);
	s = next;
    }
    if (ob->name) {
	if (d_flag)
	    debug_message("Free object %s\n", ob->name);
	if (lookup_object_hash(ob->name) == ob)
	    fatal("Freeing object %s but name still in name table", ob->name);
	free(ob->name);
	ob->name = 0;
    }
#if 0 /* Variables are now allocated together with the struct object */
    if (ob->variables) {
	free((char *)ob->variables);
	ob->variables = 0;
    }
#endif
    tot_alloc_object--;
    free((char *)ob);
}

void add_ref(struct object *ob, char *from)
{
    ob->ref++;
    if (d_flag)
	debug_message("Add reference to object %s: %d (%s)\n", ob->name,
		      ob->ref, from);
}

struct object *get_empty_object(int num_var)
{
    struct object *ob;
    int size =
	sizeof (struct object) + (num_var - 1) * sizeof (struct svalue);
    int i;

    tot_alloc_object++;
    ob = (struct object *)xalloc(size);
    memset((char *)ob, '\0', size);
    ob->ref = 1;
    ob->swap_num = -1;		/* Object not swapped. */
    ob->num_variables = num_var;
    for (i=0; i<num_var; i++) {
	ob->variables[i].type = T_NUMBER;
	/* ob->variables[i].u.number = 0;  Done with memset above */
    }
    return ob;
}

void dump_all_objects()
{
    struct object *ob;
    FILE *d;

    d = fopen("OBJ_DUMP", "w");
    if (!d) {
        add_message("Couldn't open dump file.\n");
	return;
    }
    add_message("Dumping data to 'OBJ_DUMP'... ");
    for (ob = obj_list; ob; ob = ob->next_all) {
	fprintf(d, "%s (%p)  ", ob->name, (void *)ob);
	if (ob->super)
	    fprintf(d, "Super %s (%p)", ob->super->name, (void *)ob->super);
	fprintf(d, "\t Ref %2d Rst %d Enbl_cmd %d Clnd %d Hrt_bt %d lang ref %d swp %d hp %p\n",
		ob->ref, ob->reset, ob->enable_commands, ob->cloned,
		ob->enable_heart_beat, ob->prog == 0 ? 1 : ob->prog->num_ref,
		ob->swapped,
		(void *)ob->heart_beat);
    }
    fclose(d);
    add_message("DONE\n");
}

/*
 * This one is used for extreme emergency.
 */
void verify(char *str, char *where)
{
    static struct object *harry;
    struct object *ob;
    int num_ref, i;
    int count_value_ref PROT((struct object *));

    if (!harry)
	harry = find_living_object(str, 1);
    if (!harry)
	return;
    if (harry->ref == 0) {
	debug_message("%20s verify %s ref 0\n", where, str);
	harry = 0;
	return;
    }
    num_ref = 0;
    for (ob = obj_list; ob; ob = ob->next_all) {
	if (ob->num_variables == 0)
	    continue;
	for (i=0; i < ob->num_variables; i++) {
	    if (ob->variables[i].type == T_OBJECT &&
		ob->variables[i].u.ob == harry) {
		num_ref++;
		debug_message("var in '%s'\n", ob->name);
	    }
	}
    }
    i = count_value_ref(harry);
    debug_message("%20s: num ref %2d : %2d+%2d = %2d\n", where, harry->ref,
		  num_ref, i, i + num_ref);
}

void remove_all_objects() {
    struct object *ob;
    struct value v;

    v.type = T_OBJECT;
    while(1) {
	if (obj_list == 0)
	    break;
	ob = obj_list;
	ob->inherited = 0;	/* Force destruction */
	v.u.ob = ob;
	destruct_object(&v);
	if (ob == obj_list)
	    break;
    }
    remove_destructed_objects();
}

/*
 * For debugging purposes.
 */
void check_ob_ref(struct object *ob, char *from)
{
    struct object *o;
    int i;

    if (!ob->inherited)
	return;
    for (o = obj_list, i=0; o; o = o->next_all) {
	if (o->inherit == ob)
	    i++;
    }
    if (i+1 > ob->ref) {
	fprintf(stderr,
		"FATAL too many references to inherited object %s (%d) from %s.\n",
		ob->name, ob->ref, from);
	if (current_object)
	    fprintf(stderr, "current_object: %s\n", current_object->name);
	for (o = obj_list; o; o = o->next_all) {
	    if (o->inherit != ob)
		continue;
	    fprintf(stderr, "  %s\n", ob->name);
	}
    }
}

static struct object *hashed_living[LIVING_HASH_SIZE];

static int num_living_names, num_searches = 1, search_length = 1;

static int hash_living_name(char *str)
{
    unsigned ret = 0;

    while(*str)
	ret = ret * 2 + *str++;
    return ret % LIVING_HASH_SIZE;
}

struct object *find_living_object(char *str, int player)
{
    struct object **obp, *tmp;
    struct object **hl;

    num_searches++;
    hl = &hashed_living[hash_living_name(str)];
    for (obp = hl; *obp; obp = &(*obp)->next_hashed_living) {
	search_length++;
	if (player && !(*obp)->once_interactive)
	    continue;
	if ((*obp)->enable_commands == 0)
	    continue;
	if (strcmp((*obp)->living_name, str) == 0)
	    break;
    }
    if (*obp == 0)
	return 0;
    /* Move the found ob first. */
    if (obp == hl)
	return *obp;
    tmp = *obp;
    *obp = tmp->next_hashed_living;
    tmp->next_hashed_living = *hl;
    *hl = tmp;
    return tmp;
}

void set_living_name(struct object *ob, char *str)
{
    struct object **hl;
    struct value *v;


    if (ob->living_name) {
	remove_living_name(ob);
	ob->next_hashed_living = 0;
    } else {
	num_living_names++;
    }
    hl = &hashed_living[hash_living_name(str)];
    ob->next_hashed_living = *hl;
    *hl = ob;
    ob->living_name = make_shared_string(str);
    if (ob->interactive) {
	v = apply("query_level", ob, 0);
	if (v && v->type == T_NUMBER && v->u.number >= 21)
	    ob->is_wizard = 1;
    }
    return;
}

void remove_living_name(struct object *ob)
{
    struct object **hl;

    num_living_names--;
    if (!ob->living_name)
	fatal("remove_living_name: no living name set.\n");
    hl = &hashed_living[hash_living_name(ob->living_name)];
    while(*hl) {
	if (*hl == ob)
	    break;
	hl = &(*hl)->next_hashed_living;
    }
    if (*hl == 0)
	fatal("remove_living_name: Object named %s no in hash list.\n",
	      ob->living_name);
    *hl = ob->next_hashed_living;
    free_string(ob->living_name);
    ob->next_hashed_living = 0;
    ob->living_name = 0;
}

void stat_living_objects() {
    add_message("%d named objects, average search length: %4.2f\n",
		num_living_names, (double)search_length / num_searches);
}
