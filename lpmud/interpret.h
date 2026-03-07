union u {
    char *string;
    int number;
    struct object *ob;
    struct vector *vec;
    struct vector_regexp *reg;
};

/*
 * The definition of a value node. It is used for all computations.
 * Strings can be constant, in which case they must not be freed.
 */
struct value {
    short type;
    short constant;		/* This string is not alloced with malloc */
    struct value *next;		/* Linked list of values. */
    union u u;
};

/*
 * This structure is equivalent with struct value but with one exception.
 * The 'next' pointer is not here. This will save space for all global
 * variables in objects, as the pointer is not needed for them.
 */
struct svalue {
    int type;
    union u u;
};

#define T_INVALID	0
#define T_NUMBER	1
#define T_STRING	2
#define T_POINTER	3
#define T_OBJECT	4

struct lnode_variable;
struct value *alloc_value(),
    *apply PROT((char *, struct object *, struct value *));
struct svalue *find_value PROT((struct lnode_variable *));
struct value *make_number PROT((int)), *make_string PROT ((char *));

extern struct value const0, const1;

struct vector {
    short size;
    short ref;
    struct wiz_list *wl;	/* Save who made the vector */
    struct svalue item[1];
};

#define ALLOC_VECTOR(nelem) \
    (struct vector *)xalloc(sizeof (struct vector) + \
			    sizeof(struct svalue) * (nelem - 1))

/*
 * regexp compiled code is stored in vector of size -1.
 * This is not implemented yet.
 */
struct vector_regexp {
    struct vector vector;
    struct regexp *regexp;
};

struct lnode_def;
void free_vector PROT((struct vector *)), free_all_values(),
    call_function PROT((struct lnode_def *));
