	/* next error will still return 1 by default */
%{
#include "y.tab.h"
#include "string.h"
#ifndef PROT
#define PROT(x) x
#endif
#include "lnode.h"
#include "config.h"

/*
 * Flex doesn't handle negative characters, but this change will at least
 * not crash the game.
 */
#ifdef YY_FATAL_ERROR
#undef YY_FATAL_ERROR
#define YY_FATAL_ERROR(msg) \
	{ \
	fputs( msg, stderr ); \
	putc( '\n', stderr ); \
	error(msg); \
	}
#endif

#define RESBIT 0x4000

int current_line;
int total_lines;	/* Used to compute average compiled lines/s */
char *current_file;
extern char *argument_name;
struct keyword;
int number(char *str), ident(char *str), string(char *str);
static int lookupword(char *s, struct keyword *words, int h);
int lookup_resword(char *s);
static int islocal(char *str);
void yyrestart(FILE *input_file);

extern char *local_names[];
extern int current_number_of_locals;

extern char *string_copy(char *);

void pre_processor_info(char *str), yyerror(char *str), error(char *str, ...);
%}
%option noinput nounput
%p 6000
%e 2000
%n 1000
%%
";"	{ return ';'; }
"("	{ return '('; }
")"	{ return ')'; }
","	{ return ','; }
"{"	{ return '{'; }
"}"	{ return '}'; }
"++"	{ return F_INC; }
"--"	{ return F_DEC; }
"~"	{ return '~'; }
"&"	{ return '&'; }
"&="	{ return F_AND_EQ; }
"|"	{ return '|'; }
"|="	{ return F_OR_EQ; }
"^"	{ return '^'; }
"^="	{ return F_XOR_EQ; }
"<<"	{ return F_LSH; }
"<<="	{ return F_LSH_EQ; }
">>"	{ return F_RSH; }
">>="	{ return F_RSH_EQ; }
"!"	{ return F_NOT; }
"&&"	{ return F_LAND; }
"||"	{ return F_LOR; }
"->"	{ return F_ARROW; }
"="	{ return '='; }
"-"	{ return '-'; }
"-="	{ return F_SUB_EQ; }
"+"	{ return '+'; }
"+="	{ return F_ADD_EQ; }
"*"	{ return '*'; }
"*="	{ return F_MULT_EQ; }
"%"	{ return '%'; }
"%="	{ return F_MOD_EQ; }
"/"	{ return '/'; }
"/="	{ return F_DIV_EQ; }
"<"	{ return '<'; }
">"	{ return '>'; }
">="	{ return F_GE; }
"<="	{ return F_LE; }
"=="	{ return F_EQ; }
"!="	{ return F_NE; }
"::"	{ return F_COLON_COLON; }
^#.*$	{ pre_processor_info(yytext); }
"["	{ return '['; }
"]"	{ return ']'; }
[0-9]+	{ return number(yytext); }
'.'	{ yylval.lnode = (struct lnode *)alloc_lnode_number(F_NUMBER,
							    yytext[1]);
          return F_NUMBER; }
[_a-zA-Z][_a-zA-Z0-9]* { 
          int r;

          r = lookup_resword(yytext);
          if (r >= 0) {
             return r;
          } else
             return ident(yytext);
        }
\"([^"\n]|\\\")*\"    { return string(yytext); }
[\t ]	{ break; }
"\n"	{ current_line++;
	  if (current_line & L_MASK) {
	      yyerror("Too many lines.\n");
	      current_line = 0;
	  }
	  total_lines++;
	}
.	{ char buff[100]; sprintf(buff, "Illegal character '%c'", yytext[0]);
	  yyerror(buff); }
%%

char *xalloc(size_t);

extern YYSTYPE yylval;

#if defined(LEX) || defined(FLEX)
int yywrap(void) { return 1; }
#endif

static int islocal(char *str)
{
    int i;

    for (i=current_number_of_locals-1; i>=0; i--) {
	if (strcmp(local_names[i], str) == 0)
            return i;
    }
    return -1;
}

int ident(char *str)
{
    int i;
   
    i = islocal(str);
    if (i >= 0) {
        yylval.lnode = (struct lnode *)alloc_lnode_number(F_LOCAL_NAME, i);
        return F_LOCAL_NAME;
    }
/*
    if (argument_name && strcmp(str, argument_name) == 0)
	return F_ARGUMENT;
*/
    yylval.string = string_copy(str);
    return F_IDENTIFIER;
}

int string(char *str)
{
    char *p;

    p = xalloc(strlen(str) - 1);
    yylval.string = p;
    for (str++; str[1] != '\0'; str++, p++) {
	if (str[0] == '\\' && str[1] == 'n') {
	    *p = '\n';
	    str++;
	} else if (str[0] == '\\' && str[1] == 't') {
	    *p = '\t';
	    str++;
	} else if (str[0] == '\\' && str[1] == '"') {	/* LA */
	    *p = '"';					/* LA */
	    str++;					/* LA */
	    if (!str[1]) {
		yyerror("Unterminated string");
		break;
	    }
	} else if (str[0] == '\\') {
	    *p = str[1];
	    str++;
	} else
	    *p = *str;
    }
    *p = '\0';
    return F_STRING;
}

int number(char *str)
{
    int i;

    i = atoi(str);
    if (i == 0) {
        yylval.lnode = (struct lnode *)alloc_lnode_single(F_CONST0);
	return F_CONST0;
    } else if (i == 1) {
	yylval.lnode = (struct lnode *)alloc_lnode_single(F_CONST1);
	return F_CONST1;
    }
    yylval.lnode =(struct lnode *)alloc_lnode_number(F_NUMBER, atoi(str));
    return F_NUMBER;
}

void start_new_file(FILE *f)
{
    yyrestart(f);
    yyin = f;
}

void pre_processor_info(char *str)
{
    int line, i;
    char file[100];
    if (sscanf(str, "# %d \"%99s\"", &line, file) != 2)
	return;
    i = strlen(file);
    if (i > 0 && file[i-1] == '"')
	file[i-1] = '\0';
    free(current_file);
    current_file = string_copy(file);
    current_line = line - 1;
}

static struct keyword {
    char *word;
    int  token;
} predefs[] = {
{ "add_action",		F_ADD_ACTION, },
{ "add_adj",		F_ADD_ADJ, },
{ "add_subst",		F_ADD_SUBST, },
{ "add_verb",		F_ADD_VERB, },
{ "add_worth",		F_ADD_WORTH, },
{ "allocate",		F_ALLOCATE, },
{ "call_other",		F_CALL_OTHER, },
{ "call_out",		F_CALL_OUT, },
{ "capitalize",		F_CAPITALIZE, },
{ "cat",		F_CAT, },
{ "catch",		F_CATCH, },
{ "clear_bit",		F_CLEAR_BIT, },
{ "clone_object",	F_CLONE_OBJECT, },
{ "combine_free_list",	F_COMBINE_FREE_LIST, },
{ "command",		F_COMMAND, },
{ "create_wizard",	F_CREATE_WIZARD, },
{ "creator",		F_CREATOR, },
{ "crypt",		F_CRYPT, },
{ "ctime",		F_CTIME, },
{ "destruct",		F_DESTRUCT, },
{ "disable_commands",	F_DISABLE_COMMANDS, },
{ "ed",			F_ED, },
{ "enable_commands",	F_ENABLE_COMMANDS, },
{ "environment",	F_ENVIRONMENT, },
{ "explode",		F_EXPLODE, },
{ "extract",		F_EXTRACT, },
{ "file_name",		F_FILE_NAME, },
{ "file_size",		F_FILE_SIZE, },
{ "filter_objects",	F_FILTER_OBJECTS, },
{ "find_living",	F_FIND_LIVING, },
{ "find_object",	F_FIND_OBJECT, },
{ "find_player",	F_FIND_PLAYER, },
{ "first_inventory",	F_FIRST_INVENTORY, },
{ "implode",		F_IMPLODE, },
{ "input_to",		F_INPUT_TO, },
{ "intp",		F_INTP, },
{ "living",		F_LIVING, },
{ "localcmd",		F_LOCALCMD, },
{ "log_file",		F_LOG_FILE, },
{ "lower_case",		F_LOWER_CASE, },
{ "ls",			F_LS, },
{ "mkdir",		F_MKDIR, },
{ "move_object",	F_MOVE_OBJECT, },
{ "next_inventory",	F_NEXT_INVENTORY, },
{ "notify_fail",	F_NOTIFY_FAIL, },
{ "objectp",		F_OBJECTP, },
{ "parse_command",	F_PARSE_COMMAND, },
{ "people",		F_PEOPLE, },
{ "pointerp",		F_POINTERP, },
{ "present",		F_PRESENT, },
{ "previous_object",	F_PREVIOUS_OBJECT, },
{ "query_host_name",	F_QUERY_HOST_NAME, },
{ "query_idle",		F_QUERY_IDLE, },
{ "query_ip_number",	F_QUERY_IP_NUMBER, },
{ "query_load_average",	F_QUERY_LOAD_AVERAGE, },
{ "query_snoop",	F_QUERY_SNOOP, },
{ "query_verb",		F_QUERY_VERB, },
{ "random",		F_RANDOM, },
{ "regcomp",		F_REGCOMP, },
{ "regexec",		F_REGEXEC, },
{ "remove_call_out",	F_REMOVE_CALL_OUT, },
{ "restore_object",	F_RESTORE_OBJECT, },
{ "rm",			F_RM, },
{ "rmdir",		F_RMDIR, },
{ "rusage",		F_RUSAGE, },
{ "save_object",	F_SAVE_OBJECT, },
{ "say",		F_SAY, },
{ "set_bit",		F_SET_BIT, },
{ "set_heart_beat",	F_SET_HEART_BEAT, },
{ "set_light",		F_SET_LIGHT, },
{ "set_living_name",	F_SET_LIVING_NAME, },
{ "shout",		F_SHOUT, },
{ "shutdown",		F_SHUTDOWN, },
{ "sizeof",		F_SIZEOF, },
{ "snoop",		F_SNOOP, },
{ "sscanf",		F_SSCANF, },
{ "stringp",		F_STRINGP, },
{ "strlen",		F_STRLEN, },
{ "swap",		F_SWAP, },
{ "tail",		F_TAIL, },
{ "tell_object",	F_TELL_OBJECT, },
{ "tell_room",		F_TELL_ROOM, },
{ "test_bit",		F_TEST_BIT, },
{ "this_object",	F_THIS_OBJECT, },
{ "this_player",	F_THIS_PLAYER, },
{ "throw",		F_THROW, },
{ "time",		F_TIME, },
{ "transfer",		F_TRANSFER, },
{ "users",		F_USERS, },
{ "wizlist",		F_WIZLIST, },
{ "write",		F_WRITE, },
{ "write_file",		F_WRITE_FILE, },
};
#define NELEM(a) (sizeof (a) / sizeof (a)[0])

static struct keyword reswords[] = {
{ "break",		F_BREAK, },
{ "continue",		F_CONTINUE, },
{ "do",			F_DO, },
{ "else",		F_ELSE, },
{ "for",		F_FOR, },
{ "if",			F_IF, },
{ "inherit",		F_INHERIT, },
{ "int",		F_INT, },
{ "object",		F_STATUS, },
{ "return",		F_RETURN, },
{ "static",		F_STATIC, },
{ "status",		F_STATUS, },
{ "string",		F_STRING_DECL, },
{ "while",		F_WHILE, },
};

static int
lookupword(char *s, struct keyword *words, int h)
{
    int i, l, r;

    l = 0;
    h--;
    while (l <= h) {
	i = l + (h - l) / 2;
	r = strcmp(s, words[i].word);
	if (r == 0)
	    return words[i].token;
	if (r < 0)
	    h = i - 1;
	else
	    l = i + 1;
    }
    return -1;
}

int lookup_resword(char *s)
{
    return lookupword(s, reswords, NELEM(reswords));
}

int lookup_predef(char *s)
{
    return lookupword(s, predefs, NELEM(predefs));
}

#if 0
"ls"	{ return F_LS; }
"cat"	{ return F_CAT; }
query_snoop { return F_QUERY_SNOOP; }
set_bit	    { return F_SET_BIT; }
clear_bit   { return F_CLEAR_BIT; }
test_bit    { return F_TEST_BIT; }
catch	    { return F_CATCH; }
throw	    { return F_THROW; }
filter_objects { return F_FILTER_OBJECTS; }
"rm"	{ return F_RM; }
"log_file" { return F_LOG_FILE; }
write_file { return F_WRITE_FILE; }
file_size  { return F_FILE_SIZE; }
"else"	{ return F_ELSE; }
"find_living" { return F_FIND_LIVING; }
find_player   { return F_FIND_PLAYER; }
"previous_object" { return F_PREVIOUS_OBJECT; }
"inherit"     { return F_INHERIT; }
"tell_object" { return F_TELL_OBJECT; }
"people" { return F_PEOPLE; }
parse_command   { return F_PARSE_COMMAND; }
set_living_name { return F_SET_LIVING_NAME; }
query_host_name { return F_QUERY_HOST_NAME; }
tail	 { return F_TAIL; }
file_name { return F_FILE_NAME; }
query_verb { return F_QUERY_VERB; }
"snoop" { return F_SNOOP; }
"input_to" { return F_INPUT_TO; }
"wizlist" { return F_WIZLIST; }
rmdir   { return F_RMDIR; }
mkdir   { return F_MKDIR; }
"crypt" { return F_CRYPT; }
"first_inventory" { return F_FIRST_INVENTORY; }
"regcomp"	 { return F_REGCOMP; }
"regexec"	 { return F_REGEXEC; }
"transfer"	 { return F_TRANSFER; }
"next_inventory" { return F_NEXT_INVENTORY; }
"string" { return F_STRING_DECL; }
"shout"	{ return F_SHOUT; }
"sscanf" { return F_SSCANF; }
"extract" { return F_EXTRACT; }
"ed"	{ return F_ED; }
"localcmd" { return F_LOCALCMD; }
"swap"	{ return F_SWAP; }
explode { return F_EXPLODE; }
implode { return F_IMPLODE; }
query_idle { return F_QUERY_IDLE; }
notify_fail { return F_NOTIFY_FAIL; }
query_load_average { return F_QUERY_LOAD_AVERAGE; }
"break"	{ return F_BREAK; }
intp	{ return F_INTP; }
pointerp { return F_POINTERP; }
objectp  { return F_OBJECTP; }
stringp  { return F_STRINGP; }
users   { return F_USERS; }
"ctime" { return F_CTIME; }
"disable_commands"  { return F_DISABLE_COMMANDS; }
"allocate"	    { return F_ALLOCATE; }
"combine_free_list" { return F_COMBINE_FREE_LIST; }
"continue" { return F_CONTINUE; }
"while"	{ return F_WHILE; }
"do"	{ return F_DO; }
"for"	{ return F_FOR; }
"capitalize" { return F_CAPITALIZE; }
"living" { return F_LIVING; }
"lower_case" { return F_LOWER_CASE; }
"set_heart_beat" { return F_SET_HEART_BEAT; }
"strlen" { return F_STRLEN; }
"say"	{ return F_SAY; }
"tell_room"	{ return F_TELL_ROOM; }
"find_object"   { return F_FIND_OBJECT; }
"add_worth"	{ return F_ADD_WORTH; }
"time"		{ return F_TIME; }
"create_wizard" { return F_CREATE_WIZARD; }
"destruct" { return F_DESTRUCT; }
"static"   { return F_STATIC; }
"environment" { return F_ENVIRONMENT; }
"save_object" { return F_SAVE_OBJECT; }
"enable_commands" { return F_ENABLE_COMMANDS; }
"sizeof" { return F_SIZEOF; }
"random" { return F_RANDOM; }
"restore_object" { return F_RESTORE_OBJECT; }
"this_object" { return F_THIS_OBJECT; }
"clone_object" { return F_CLONE_OBJECT; }
"this_player" { return F_THIS_PLAYER; }
"if"	{ return F_IF; }
"status" { return F_STATUS; }
"object" { return F_STATUS; }
"return" { return F_RETURN; }
"write" { return F_WRITE; }
"add_verb" { return F_ADD_VERB; }
"add_adj" { return F_ADD_ADJ; }
"add_subst" { return F_ADD_SUBST; }
"add_action" { return F_ADD_ACTION; }
"move_object" { return F_MOVE_OBJECT; }
"present" { return F_PRESENT; }
"call_out" { return F_CALL_OUT; }
"remove_call_out" { return F_REMOVE_CALL_OUT; }
"creator" { return F_CREATOR; }
"query_ip_number" { return F_QUERY_IP_NUMBER; }
"command" { return F_COMMAND; }
"set_light" { return F_SET_LIGHT; }
"int"	{ return F_INT; }
"call_other" { return F_CALL_OTHER; }
"shutdown" { return F_SHUTDOWN; }
#endif
