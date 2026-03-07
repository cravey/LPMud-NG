/*
 * Definition of an object.
 * If the object is inherited, then it must not be destructed !
 *
 * The reset is used as follows:
 * 0: There is an error in the reset() in this object. Never call it again.
 * 1: Normal state.
 * 2 or higher: This is an interactive player, that has not given any commands
 *		for a number of reset periods.
 *
 * In the beginning, 'prog' points to the program tree, and 'status' to the
 * linked list of variable definitions.
 * After the object has been loaded back from the swap, the program tree
 * and variable deinitions will be stored in one sequential memory area.
 * This area will then be pointed to by 'block'.
 * If it is the case, free_prog() must not be called
 * with ob->prog as address, but rather free() should be called (once) with
 * ob->block.
 */
struct object {
    char enable_heart_beat;	/* Call heart_beat or not. */
    char reset;			/* True if object has been reset */
    char enable_commands;	/* Enable usage of sentence commands */
    char cloned;		/* A cloned object. */
    char name_length;		/* Number of characters in name */
    char destructed;		/* True when the objects is destructed */
    char swapped;		/* True if prog is swapped out */
    char not_touched;		/* how used the object is */
    char inherited;		/* Has someone inherited this object ? */
    char is_wizard;		/* Used for wizlist only */
    char once_interactive;	/* Has once been onteractive. */
    short total_light;
    short num_variables;	/* Number of variables */
    int ref;			/* Reference count. */
    char *block;		/* Pointer to the start of the prog block */
    int swap_num;		/* Swap file offset. -1 is not swapped yet. */
    unsigned int swap_size;	/* Size of object on swap area. */
    struct lnode_def *prog;
    struct lnode_var_def *status;
    struct lnode *heart_beat;
    char *name;
    struct object *next_all, *next_inv, *next_heart_beat, *next_hash;
    struct object *contains;
    struct object *super;		/* Which object surround us ? */
    struct interactive *interactive;	/* Data about an interactive player */
    struct sentence *sent;
    struct ed_buffer *ed_buffer;	/* Local ed */
    struct wiz_list *wl;		/* What wizard defined this object */
    struct object *inherit;		/* Where to inherit from */
    struct object *next_hashed_living;
    char *living_name;			/* Name of living object if in hash */
    struct svalue variables[1];		/* All variables to this program */
    /* The variables must come last in the struct */
};

extern struct object *load_object PROT((char *)), *find_object PROT((char *));
extern struct object *get_empty_object PROT((int)),
	*find_object2 PROT((char *));
extern struct object *current_object, *command_giver;

extern struct object *obj_list;

struct value;
void destruct_object PROT((struct value *));
void remove_destructed_objects(), save_object PROT((struct object *, char *)),
    move_object PROT((struct object *, struct object *)),
    tell_object PROT((struct object *, char *)),
    tell_npc PROT((struct object *, char *)),
    add_ref PROT((struct object *, char *)),
    free_object PROT((struct object *, char *));

struct value *clone_object PROT((char *));

int restore_object PROT((struct object *, char *));
