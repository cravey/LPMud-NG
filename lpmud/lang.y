%{
/*
 * This is the grammar definition of LPC.
 */
#include <string.h>
#include <stdio.h>
#include "lint.h"
#include "lnode.h"
#include "config.h"
#include "interpret.h"
#include "object.h"

#define YYMAXDEPTH	600

struct lnode *prog, *heart_beat;
int variable_count;
void yyerror(char *str);
void free_all_local_names(void);
void add_local_name(char *str);
void smart_log(char *error_file, int line, char *what);
extern int yylex(void), check_deklared(char *str);
extern int lookup_predef(char *s), defined_function(char *s);
static void copy_variables(struct object *from);
int static_variable_flag;

char *argument_name;	/* The name of the current argument. */
char *xalloc(size_t), *string_copy(char *);

extern int current_line;
/*
 * 'inherit_file' is used as a flag. If it is set to a string
 * after yyparse(), this string should be loaded as an object,
 * and the original object must be loaded again.
 * 'inherit_ob' will point to the super class object. This value is saved
 * so that load_object() can set up pointers to it.
 */
extern char *current_file, *inherit_file;

extern struct object *find_object2(char *);

struct object *inherit_ob;

char *local_names[MAX_LOCAL];
int current_number_of_locals = 0;
static int num_arg;
struct lnode_def *current_functions = 0;
%}

%token F_THIS_PLAYER F_IF F_IDENTIFIER F_LAND F_LOR F_STATUS F_SET_TRUE
%token F_SET_FALSE F_CONS F_RETURN F_NOT F_WRITE F_STRING F_ADD_VERB
%token F_ADD_ADJ F_ADD_SUBST F_ADD_ACTION F_MOVE_OBJECT F_INC F_DEC
%token F_POST_INC F_POST_DEC F_COMMA
%token F_NUMBER F_ASSIGN F_INT F_CALL_OTHER F_ADD F_SUBTRACT F_MULTIPLY
%token F_DIVIDE F_LT F_GT F_EQ F_GE F_LE F_ARGUMENT F_FUNCTION
%token F_CLONE_OBJECT F_THIS_OBJECT F_SAVE_OBJECT F_RESTORE_OBJECT F_NE
%token F_ENVIRONMENT F_ADD_EQ F_SUB_EQ F_DIV_EQ F_MULT_EQ F_PRESENT
%token F_COMMAND F_SET_LIGHT F_DESTRUCT F_CREATE_WIZARD F_NEGATE F_SAY
%token F_STRLEN F_SUBSCRIPT F_WHILE F_BREAK F_SHUTDOWN F_LOG_FILE
%token F_DO F_FOR
%token F_SSCANF F_SHOUT F_STRING_DECL F_LOCAL_NAME F_FIRST_INVENTORY
%token F_NEXT_INVENTORY F_ENABLE_COMMANDS F_RANDOM F_INPUT_TO F_CRYPT
%token F_LS F_CAT F_FIND_LIVING F_TELL_OBJECT F_PEOPLE F_ED F_LIVING
%token F_LOWER_CASE F_ELSE F_CAPITALIZE F_SET_HEART_BEAT F_SNOOP F_TELL_ROOM
%token F_FIND_OBJECT F_WIZLIST F_RM F_CONST0 F_CONST1 F_BLOCK F_TRANSFER
%token F_REGCOMP F_REGEXEC F_LOCALCMD F_SWAP F_CONTINUE F_ADD_WORTH F_TIME
%token F_MOD F_MOD_EQ F_QUERY_IP_NUMBER F_INHERIT F_COLON_COLON F_CREATOR
%token F_STATIC F_CALL_OUT F_REMOVE_CALL_OUT F_COMBINE_FREE_LIST F_ALLOCATE
%token F_SIZEOF F_DISABLE_COMMANDS F_CTIME F_INTP F_STRINGP F_OBJECTP
%token F_POINTERP F_USERS F_ARROW F_PREVIOUS_OBJECT F_AGGREGATE F_EXTRACT
%token F_FILE_NAME F_QUERY_VERB F_TAIL F_QUERY_HOST_NAME F_EXPLODE
%token F_COMPL F_AND F_AND_EQ F_OR F_OR_EQ F_XOR F_XOR_EQ
%token F_LSH F_LSH_EQ F_RSH F_RSH_EQ F_MKDIR F_RMDIR F_QUERY_SNOOP
%token F_FIND_PLAYER F_WRITE_FILE F_FILE_SIZE F_PARSE_COMMAND F_IMPLODE
%token F_QUERY_IDLE F_QUERY_LOAD_AVERAGE F_FILTER_OBJECTS F_NOTIFY_FAIL
%token F_CATCH F_THROW F_SET_LIVING_NAME F_SET_BIT F_CLEAR_BIT F_TEST_BIT
%token F_RUSAGE

%union
{
	struct lnode *lnode;
	int number;
	char *string;
}

%type <lnode> cond expr1 expr_list expr2 expr3 function_call variable
%type <lnode> statements statement program def return expr4 expr31
%type <lnode> string expr0 F_NUMBER expr25 expr27 expr23 expr22 while do for
%type <lnode> expr24 expr211 expr212 expr213 comma_expr for_expr
%type <lnode> F_LOCAL_NAME block number F_CONST0 F_CONST1

%type <number> F_SET_FALSE F_SET_TRUE F_THIS_PLAYER F_WRITE F_THIS_OBJECT
%type <number> assign static

%type <string> F_IDENTIFIER F_STRING F_ARGUMENT function_name

%nonassoc LOWER_THAN_ELSE
%nonassoc F_ELSE

%%

all: { current_functions = 0; } program { prog = $2; };

program: program def possible_semi_colon
	{ if ($2 != 0) {
	    struct lnode_def *p = (struct lnode_def *)$2;
	    p->next = (struct lnode_def *)$1;
	    $$ = (struct lnode *)p;
#if 0
	    if (defined_function(p->name))
	      yyerror("Illegal to redeclare function.");
#endif
	    current_functions = p;
	  } else
	    $$ = $1;
        }
       |	 /* empty */ { $$ = 0; };

possible_semi_colon: /* empty */
                   | ';' { yyerror("Extra ';'. Ignored."); };

inheritance: F_INHERIT F_STRING ';'
		{
		    if (inherit_ob || inherit_file) {
			yyerror("Multiple inheritance not allowed.\n");
			free($2);
		    } else if (prog) {
			yyerror("inherit must come first.\n");
			free($2);
		    } else {
			inherit_ob = find_object2($2);
			if (inherit_ob == 0) {
			    inherit_file = $2;
			    return 0;
			}
			free($2);
			copy_variables(inherit_ob);
		    }
		}

number:  F_NUMBER | F_CONST0 | F_CONST1;

def: static F_IDENTIFIER '(' argument ')'
	{ num_arg = current_number_of_locals; }
    block
	{ struct lnode_def *p;
	  p = alloc_lnode_def(F_IDENTIFIER,$2,$7,
			      current_number_of_locals);
	  p->num_arg = num_arg;
	  if ($1)
	      p->is_static = 1;
	  $$ = (struct lnode *)p;
	  if (strcmp(p->name, "heart_beat") == 0) heart_beat = $$;
	  if (argument_name) free(argument_name);
	  argument_name = 0; free_all_local_names(); }
/*   | name '(' argument ')' block
	{ yyerror("Predefined function name.\n"); $$ = 0; }*/
   | static { static_variable_flag = $1; } type name_list ';'
            { $$ = 0; static_variable_flag = 0; }
   | inheritance { $$ = 0; } ;

static:  F_STATIC { $$ = 1; }
      |  /* empty */ { $$ = 0; }
      ;

req_semi: ';'
    	       | /* empty */ { yyerror("Missing ';'"); };

argument: /* empty */
        | local_list ;

type: F_STATUS | F_INT | F_STRING_DECL;

name_list: new_name
	 | new_name ',' name_list;

new_name: optional_star F_IDENTIFIER
	{ alloc_lnode_var_def(F_STATUS, $2, variable_count++); };

optional_star: '*' | /* empty */ ;

block: '{' local_declarations statements '}'
	{ $$ = (struct lnode *)alloc_lnode_block($3); };

local_declarations: /* empty */
		  | local_declarations type local_list req_semi ;

local_list: F_IDENTIFIER { add_local_name($1); }
	  | local_list ',' F_IDENTIFIER { add_local_name($3); }
	  | F_ARGUMENT { yyerror("Illegal to redeclare argument"); }
	  | F_LOCAL_NAME { yyerror("Illegal to redeclare local name"); };

statements: /* empty */ { $$ = 0; }
	  | statement statements
		{ if ($1 == 0)
		      $$ = $2;
		  else
		      $$ = (struct lnode *)alloc_lnode_2(F_CONS, $1, $2);
	        }
	  | error ';' { $$ = 0; };

statement: comma_expr ';' { $$ = $1; }
	 | cond | while | do | for | return ';' { $$ = $1; }
	 | block
	 | F_BREAK req_semi { $$ = (struct lnode *)alloc_lnode_single(F_BREAK);
			    };
	 | F_CONTINUE req_semi { $$ = (struct lnode *)alloc_lnode_single(F_CONTINUE);
			    };

while: F_WHILE '(' comma_expr ')' statement
     { $$ = (struct lnode *)alloc_lnode_2(F_WHILE, $3, $5); };

do: F_DO statement F_WHILE '(' comma_expr ')' req_semi
  { $$ = (struct lnode *)alloc_lnode_2(F_DO, $2, $5); };

for: F_FOR '(' for_expr ';' for_expr ';' for_expr ')' statement
   { $$ = (struct lnode *)
           alloc_lnode_2(F_CONS,$3,
			 (struct lnode *)
			 alloc_lnode_2(F_CONS,
				       (struct lnode *)alloc_lnode_3(F_FOR,$5,
								     $7,$9),
				       (struct lnode *)0)); }

for_expr: /* EMPTY */
        { $$ = (struct lnode *)alloc_lnode_single(F_CONST1); }
        | comma_expr
        { $$=$1; }

comma_expr: expr0
          { $$=$1; };
          | expr0 ',' comma_expr
          { 
	  if($3->type==F_COMMA)
	    $$ = (struct lnode *)alloc_lnode_2(F_COMMA, $1, $3);
	  else
	    $$ = (struct lnode *)alloc_lnode_2(F_COMMA, $1,
			      (struct lnode *)alloc_lnode_2(F_COMMA, $3, 0));
	}

expr0: expr1
     | variable assign expr0
     { $$ = (struct lnode *)alloc_lnode_2($2, $1, $3); }
     | error assign expr0 { yyerror("Illegal LHS"); $$ = 0; }

assign: '=' { $$ = F_ASSIGN; }
      | F_AND_EQ { $$ = F_AND_EQ; }
      | F_OR_EQ { $$ = F_OR_EQ; }
      | F_XOR_EQ { $$ = F_XOR_EQ; }
      | F_LSH_EQ { $$ = F_LSH_EQ; }
      | F_RSH_EQ { $$ = F_RSH_EQ; }
      | F_ADD_EQ { $$ = F_ADD_EQ; }
      | F_SUB_EQ { $$ = F_SUB_EQ; }
      | F_MULT_EQ { $$ = F_MULT_EQ; }
      | F_MOD_EQ { $$ = F_MOD; }
      | F_DIV_EQ { $$ = F_DIV_EQ; };

return: F_RETURN { $$ = (struct lnode *)alloc_lnode_1(F_RETURN, 0); }
      | F_RETURN comma_expr { $$ = (struct lnode *)alloc_lnode_1(F_RETURN, $2); };

expr_list: /* empty */ { $$ = 0; }
	 | expr0
	{ $$ = (struct lnode *)alloc_lnode_2(F_CONS, $1, 0); }
         | expr0 ',' expr_list
	{ $$ = (struct lnode *)alloc_lnode_2(F_CONS, $1, $3); };

expr1: expr2
     | expr2 F_LOR expr1
	{ $$ = (struct lnode *)alloc_lnode_2(F_LOR, $1, $3); };

expr2: expr211
     | expr211 F_LAND expr2
	{ $$ = (struct lnode *)alloc_lnode_2(F_LAND, $1, $3); };

expr211: expr212
       | expr211 '|' expr24
          { $$ = (struct lnode *)alloc_lnode_2(F_OR, $1, $3); };

expr212: expr213
       | expr212 '^' expr24
	  { $$ = (struct lnode *)alloc_lnode_2(F_XOR, $1, $3); };

expr213: expr22
       | expr213 '&' expr24
	  { $$ = (struct lnode *)alloc_lnode_2(F_AND, $1, $3); };

expr22: expr23
      | expr24 F_EQ expr24
	{ $$ = (struct lnode *)alloc_lnode_2(F_EQ, $1, $3); }
      | expr24 F_NE expr24
	{ $$ = (struct lnode *)alloc_lnode_2(F_NE, $1, $3); };

expr23: expr24
      | expr24 '>' expr24
	{ $$ = (struct lnode *)alloc_lnode_2(F_GT, $1, $3); }
      | expr24 F_GE expr24
	{ $$ = (struct lnode *)alloc_lnode_2(F_GE, $1, $3); }
      | expr24 '<' expr24
	{ $$ = (struct lnode *)alloc_lnode_2(F_LT, $1, $3); }
      | expr24 F_LE expr24
	{ $$ = (struct lnode *)alloc_lnode_2(F_LE, $1, $3); };

expr24: expr25
      | expr24 F_LSH expr25
	{ $$ = (struct lnode *)alloc_lnode_2(F_LSH, $1, $3); }
      | expr24 F_RSH expr25
	{ $$ = (struct lnode *)alloc_lnode_2(F_RSH, $1, $3); };

expr25: expr27
      | expr25 '+' expr27
	{ $$ = (struct lnode *)alloc_lnode_2(F_ADD, $1, $3); }
      | expr25 '-' expr27
	{ $$ = (struct lnode *)alloc_lnode_2(F_SUBTRACT, $1, $3); };

expr27: expr3
      | expr27 '*' expr3
	{ $$ = (struct lnode *)alloc_lnode_2(F_MULTIPLY, $1, $3); }
      | expr27 '%' expr3
	{ $$ = (struct lnode *)alloc_lnode_2(F_MOD, $1, $3); }
      | expr27 '/' expr3
	{ $$ = (struct lnode *)alloc_lnode_2(F_DIVIDE, $1, $3); };

expr3: expr31
     | F_INC variable
        { $$ = (struct lnode *)alloc_lnode_1(F_INC, $2); };
     | F_DEC variable
        { $$ = (struct lnode *)alloc_lnode_1(F_DEC, $2); };
     | F_NOT expr3
	{ $$ = (struct lnode *)alloc_lnode_1(F_NOT, $2); };
     | '~' expr3
	{ $$ = (struct lnode *)alloc_lnode_1(F_COMPL, $2); };
     | '-' expr3
	{ $$ = (struct lnode *)alloc_lnode_1(F_NEGATE, $2); };

expr31: expr4
      | variable F_INC
         { $$ = (struct lnode *)alloc_lnode_1(F_POST_INC, $1); };
      | variable F_DEC
         { $$ = (struct lnode *)alloc_lnode_1(F_POST_DEC, $1); };

expr4: function_call | variable | string | number
     | '(' comma_expr ')' { $$ = $2; }
     | '(' '{' expr_list '}' ')' { $$ = (struct lnode *)alloc_lnode_1(F_AGGREGATE, $3); };

variable: F_IDENTIFIER
	{ $$ = (struct lnode *)alloc_lnode_number(F_IDENTIFIER,
						    check_deklared($1));
						    free($1); }
	| F_ARGUMENT { $$ = (struct lnode *)alloc_lnode_single(F_ARGUMENT); }
        | F_LOCAL_NAME
	| expr4 '[' comma_expr ']'
	  { $$ = (struct lnode *)alloc_lnode_2(F_SUBSCRIPT, $1, $3); };

string: F_STRING
	{ $$ = (struct lnode *)alloc_lnode_name(F_STRING, $1); };

function_call: /*name '(' expr_list ')'
		{ $$ = (struct lnode *)alloc_lnode_1($1, $3); }
	     | */function_name '(' expr_list ')'
		{ 
		    int f;
		    if (defined_function($1) || (f = lookup_predef($1)) < 0)
		        $$ = (struct lnode *)alloc_lnode_funcall(F_FUNCTION, $1, $3); 
		    else
		        $$ = (struct lnode *)alloc_lnode_1(f, $3);
		}
	     | expr4 F_ARROW function_name '(' expr_list ')'
		{
		    struct lnode *p;
		    p = (struct lnode *)alloc_lnode_name(F_STRING, $3),
		    p = (struct lnode *)alloc_lnode_2(F_CONS, p, $5);
		    p = (struct lnode *)alloc_lnode_2(F_CONS, $1, p);
		    $$ = (struct lnode *)alloc_lnode_1(F_CALL_OTHER, p);
		};

function_name: F_IDENTIFIER
	     | F_COLON_COLON F_IDENTIFIER
		{
		    char *p = xalloc(strlen($2) + 3);
		    strcpy(p, "::"); strcat(p, $2); free($2);
		    $$ = p;
		};

/*
name: F_THIS_PLAYER { $$ = F_THIS_PLAYER; }
    | F_THIS_OBJECT { $$ = F_THIS_OBJECT; }
    | F_SET_FALSE { $$ = F_SET_FALSE; }
    | F_WRITE { $$ = F_WRITE; }
    | F_ADD_VERB { $$ = F_ADD_VERB; }
    | F_ADD_SUBST { $$ = F_ADD_SUBST; }
    | F_ED { $$ = F_ED; }
    | F_ADD_ADJ { $$ = F_ADD_ADJ; }
    | F_SHUTDOWN { $$ = F_SHUTDOWN; }
    | F_CALL_OTHER { $$ = F_CALL_OTHER; }
    | F_CLONE_OBJECT { $$ = F_CLONE_OBJECT; }
    | F_ADD_ACTION { $$ = F_ADD_ACTION; }
    | F_MOVE_OBJECT { $$ = F_MOVE_OBJECT; }
    | F_LOG_FILE { $$ = F_LOG_FILE; }
    | F_SET_TRUE { $$ = F_SET_TRUE; }
    | F_SAVE_OBJECT { $$ = F_SAVE_OBJECT; }
    | F_ENVIRONMENT { $$ = F_ENVIRONMENT; }
    | F_PRESENT { $$ = F_PRESENT; }
    | F_CREATOR { $$ = F_CREATOR; }
    | F_SET_HEART_BEAT { $$ = F_SET_HEART_BEAT; }
    | F_COMMAND { $$ = F_COMMAND; }
    | F_FILE_NAME { $$ = F_FILE_NAME; }
    | F_QUERY_VERB { $$ = F_QUERY_VERB; }
    | F_SET_BIT	   { $$ = F_SET_BIT; }
    | F_CLEAR_BIT  { $$ = F_CLEAR_BIT; }
    | F_TEST_BIT   { $$ = F_TEST_BIT; }
    | F_THROW	   { $$ = F_THROW; }
    | F_CATCH	   { $$ = F_CATCH; }
    | F_PARSE_COMMAND { $$ = F_PARSE_COMMAND; }
    | F_SET_LIVING_NAME { $$ = F_SET_LIVING_NAME; }
    | F_EXPLODE { $$ = F_EXPLODE; }
    | F_IMPLODE { $$ = F_IMPLODE; }
    | F_QUERY_IDLE { $$ = F_QUERY_IDLE; }
    | F_NOTIFY_FAIL { $$ = F_NOTIFY_FAIL; }
    | F_QUERY_LOAD_AVERAGE { $$ = F_QUERY_LOAD_AVERAGE; }
    | F_FILTER_OBJECTS { $$ = F_FILTER_OBJECTS; }
    | F_TAIL { $$ = F_TAIL; }
    | F_MKDIR { $$ = F_MKDIR; }
    | F_RMDIR { $$ = F_RMDIR; }
    | F_QUERY_SNOOP { $$ = F_QUERY_SNOOP; }
    | F_QUERY_HOST_NAME { $$ = F_QUERY_HOST_NAME; }
    | F_PREVIOUS_OBJECT { $$ = F_PREVIOUS_OBJECT; }
    | F_CRYPT { $$ = F_CRYPT; }
    | F_CAT { $$ = F_CAT; }
    | F_LS { $$ = F_LS; }
    | F_RM { $$ = F_RM; }
    | F_FIND_LIVING { $$ = F_FIND_LIVING; }
    | F_FIND_PLAYER { $$ = F_FIND_PLAYER; }
    | F_WRITE_FILE  { $$ = F_WRITE_FILE; }
    | F_FILE_SIZE   { $$ = F_FILE_SIZE; }
    | F_LOWER_CASE { $$ = F_LOWER_CASE; }
    | F_TELL_OBJECT { $$ = F_TELL_OBJECT; }
    | F_FIND_OBJECT { $$ = F_FIND_OBJECT; }
    | F_WIZLIST { $$ = F_WIZLIST; }
    | F_TRANSFER { $$ = F_TRANSFER; }
    | F_REGCOMP { $$ = F_REGCOMP; }
    | F_REGEXEC { $$ = F_REGEXEC; }
    | F_LOCALCMD { $$ = F_LOCALCMD; }
    | F_SWAP { $$ = F_SWAP; }
    | F_ADD_WORTH { $$ = F_ADD_WORTH; }
    | F_TIME { $$ = F_TIME; }
    | F_PEOPLE { $$ = F_PEOPLE; }
    | F_INPUT_TO { $$ = F_INPUT_TO; }
    | F_ENABLE_COMMANDS { $$ = F_ENABLE_COMMANDS; }
    | F_CAPITALIZE { $$ = F_CAPITALIZE; }
    | F_LIVING { $$ = F_LIVING; }
    | F_RANDOM { $$ = F_RANDOM; }
    | F_SNOOP { $$ = F_SNOOP; }
    | F_FIRST_INVENTORY { $$ = F_FIRST_INVENTORY; }
    | F_NEXT_INVENTORY { $$ = F_NEXT_INVENTORY; }
    | F_SET_LIGHT { $$ = F_SET_LIGHT; }
    | F_DESTRUCT { $$ = F_DESTRUCT; }
    | F_CREATE_WIZARD { $$ = F_CREATE_WIZARD; }
    | F_SAY { $$ = F_SAY; }
    | F_TELL_ROOM { $$ = F_TELL_ROOM; }
    | F_SHOUT { $$ = F_SHOUT; }
    | F_RESTORE_OBJECT { $$ = F_RESTORE_OBJECT; }
    | F_SSCANF { $$ = F_SSCANF; }
    | F_QUERY_IP_NUMBER { $$ = F_QUERY_IP_NUMBER; }
    | F_ALLOCATE { $$ = F_ALLOCATE; }
    | F_SIZEOF { $$ = F_SIZEOF; }
    | F_CTIME { $$ = F_CTIME; }
    | F_DISABLE_COMMANDS { $$ = F_DISABLE_COMMANDS; }
    | F_CALL_OUT { $$ = F_CALL_OUT; }
    | F_STRINGP { $$ = F_STRINGP; }
    | F_INTP { $$ = F_INTP; }
    | F_OBJECTP { $$ = F_OBJECTP; }
    | F_POINTERP { $$ = F_POINTERP; }
    | F_USERS { $$ = F_USERS; }
    | F_EXTRACT { $$ = F_EXTRACT; }
    | F_REMOVE_CALL_OUT { $$ = F_REMOVE_CALL_OUT; }
    | F_COMBINE_FREE_LIST { $$ = F_COMBINE_FREE_LIST; }
    | F_STRLEN { $$ = F_STRLEN; };
*/

cond: F_IF '(' comma_expr ')' statement %prec LOWER_THAN_ELSE
	{ $$ = (struct lnode *)alloc_lnode_3(F_IF, $3, $5, 0); }
    | F_IF '(' comma_expr ')' statement F_ELSE statement
	{ $$ = (struct lnode *)alloc_lnode_3(F_IF, $3, $5, $7); };
%%

void yyerror(char *str)
{
    extern int num_parse_error;

    if (num_parse_error > 5)
	return;
    (void)fprintf(stderr, "%s: %s line %d\n", current_file, str,
		  current_line);
    fflush(stderr);
    smart_log(current_file, current_line, str);
    if (num_parse_error == 0)
	save_error(str, current_file, current_line);
    num_parse_error++;
}

int check_declared(char *str)
{
    struct lnode_var_def *p;

    for (p = prog_status; p; p = p->next) {
	if (strcmp(p->name, str) == 0)
	    return p->num_var;
    }
    return -1;
}

int check_deklared(char *str)
{
    char buff[100];
    int r;

    r = check_declared(str);
    if (r < 0) {
        (void)sprintf(buff, "Variable %s not declared !", str);
        yyerror(buff);
    }
    return r;
}

void free_all_local_names()
{
    int i;

    for (i=0; i<current_number_of_locals; i++) {
	free(local_names[i]);
	local_names[i] = 0;
    }
    current_number_of_locals = 0;
}

void add_local_name(char *str)
{
    if (current_number_of_locals == MAX_LOCAL)
	yyerror("Too many local variables\n");
    else
	local_names[current_number_of_locals++] = str;
}

int num_val(struct lnode_number *p)
{
    switch(p->type) {
    case F_NUMBER:
        return p->number;
    case F_CONST0:
	return 0;
    case F_CONST1:
	return 1;
    default:
	fatal("Illegal type of number.\n");
    }
    /*NOTREACHED*/
    return 0;
}

/*
 * It could be possible to do '#include "/etc/passwd"' in the definition
 * of an object. The compilation would fail, but the .i file would remain
 * with interesting data.
 * Prevent this by checking the name of teh include file.
 */

int illegal_include_file(char *str)
{
    char *p;
    int i;

    if (str[0] == '/')
	return 1;
    if (strlen(str) < 3)
	return 0;
    for (p = str + 3, i = 0; *p; p++)
	if (p[0] == '/' &&  p[-1] == '.' && p[-2] == '.' && p[-3] == '/')
	    i += 1;
    if (i > 3)
	return 1;
    return 0;
}

static void copy_one_variable(struct lnode_var_def *p)
{
    if (p->next)
	copy_one_variable(p->next);
    static_variable_flag = p->is_static;
    alloc_lnode_var_def(F_STATUS, string_copy(p->name), variable_count++);
}

static void copy_variables(struct object *from)
{
    if (variable_count > 0)
	yyerror("Illegal to declare variables before the inherit statement");
    if (!from->status)
	return;
    copy_one_variable(from->status);
}

int defined_function(char *s)
{
    struct lnode_def *p;

    for(p = current_functions; p; p = p->next) {
        if (strcmp(p->name, s) == 0)
	    return 1;
    }
    return 0;
}
