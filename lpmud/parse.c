/*

  Pattern Parser package for LPmud, JnA 1990

  Ver 2.0

  If you have questions or complains about this code please refer them
  to jna@cd.chalmers.se

*/
#include <stdio.h>
#include <setjmp.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "lint.h"
#include "y.tab.h"
#include "lnode.h"
#include "interpret.h"
#include "config.h"
#include "object.h"
#include "wiz_list.h"

#define VALUE struct value
#define OBJECT struct object
#define LNODE struct lnode
#define VECTOR struct vector
#define SVALUE struct svalue

/* Functions defined in this module
*/
VALUE *parse PROT((struct lnode *l));
VALUE *filter PROT((VALUE *arr,char *func,OBJECT *ob)); 
VALUE *describe_items PROT((VALUE *arr,char *func,int live));
extern VALUE *allocate_array PROT((int));
extern VALUE *get_one_expr PROT((LNODE **, char *));
extern void assign PROT((LNODE *, VALUE *));
extern char *string_copy PROT((char *));
#ifdef DRAND48
     extern double drand48();
#endif

/* Functions used in the rest of LPmud

   assign
   assign_svalue
   alloc_value
   allocate_array
   apply
   make_string
   get_one_expr
   copy_svalue

*/

/* Constants used from LPmud

   const0
   const1
*/

/*****************************************************

  This is the parser


  Left to do 901203:

  Fix so alternatives possible after %s in a pattern 

*/
/*
  Doc for LPC function

int parse_command(string,ob/arr,string,destargs...)

	parse		Returns 1 if pattern matches

	string		Given command
	ob/arr		if arr 
				array holding the accessible objects
			if ob
				object from which to recurse and create
				the list of accessible objects, normally
				ob = environment(this_player())
	string		Parsepattern as list of words and formats:
			Example string = " 'get' / 'take' %i "
			Syntax:
				'word' 		obligatory text
				[word]		optional text
				/		Alternative marker
				%o		Single item, object
				%l		Single living object
				%s		Any text
				%w              Any word
				%p		Preposition
				%i		Any items
				%d              Number 0- or tx(0-99)

	destargs	This is the list of result variables as in sscanf
			One variable is needed for each %_
			The return types of different %_ is:
			%o	Returns an object
			%l	Returns an object
			%s	Returns a string of words
			%w      Returns a string of one word
			%p	Can on entry hold a list of word in array
				or an empty variable
				Returns:
				   if empty variable: a string
				   if array: array[0]=matched word
			%i	Returns a special array on the form:
				[0] = (int) +(wanted) -(order) 0(all)
				[1..n] (object) Objectpointers
			%d      Returns a number
			
Example:

 parse_command("take apple",environment(this_player()),
 " 'get' / 'take' %i ",items);


*/ 

/* Hardcoded function names to call in LPC objects
*/
#define QIDFUNC "id"                /* Func to query in objects to ack name */
#define QPLURIDFUNC "plural_id"     /* As Id but pluralform of name */
#define QADJFUNC "adjectiv_id"      /* As Id but ack adjectiv not name */
#define QSHORTFUNC "short"          /* Objects shortdescription */

#define EQ(x,y) (strcmp(x,y)==0)
#define EQN(x,y) (strncmp(x,y,strlen(x))==0)
#define EMPTY(x) (strcmp(x,"")==0)
#define COPY(x) (strcpy((char*)xalloc(strlen(x)+1),x))

#define KLUDGELEN   200            /* How long should strings be */

struct altern_objects {
  OBJECT *ao_obj;
  struct altern_objects *next;
};

int gDebug=0;
char gMword[KLUDGELEN];            /* Text inom '' och [] */
char gFword[KLUDGELEN];            /* Temp word gotten by getfirst() */
struct altern_objects *gOblist;    /* List of accessible objects */
char gAdjective[4*KLUDGELEN];      /* all adjectives before objname */
LNODE *gCarg;                      /* Current argument to %_ */
int gWantnum;                      /* Number of wanted items 0 = all */
struct altern_objects *gPobjects;  /* List of parsed objects */
LNODE *gTxarg;                     /* Argument of LPCvariable to store %s  */
LNODE *gForprepos;                 /* Save lnode* here for findprepos */

#define EP 0          /* End Parse marker */
#define SI 1          /* %o single item */
#define IT 2          /* %i  items */
#define US 3          /* %l  single living object */
#define PP 4          /* %p  prepositions */
#define TX 5          /* %s  string of words */
#define DTX 6         /* 'word' */
#define OTX 7         /* [word] */
#define ALT 8         /* /   alternates */
#define W1 9          /* %w  string of one word */
#define NUM 10        /* %d  integer */

/* Search string in reverse order, I should replace it with strrchr I know :)
*/
char *backstrchr(char *apa, char ch)
{
  char *sp;

  sp=&apa[strlen(apa)];
  while (sp>apa) {
    if (*sp==ch) return sp+1;
    sp--;
  }
  return 0;
}

/* Convert string to lowercase
*/
char *lowercase(char *apa)
{
  char *bepa;

  bepa=apa;
  while (*bepa) {
    if ((*bepa>'A') && (*bepa<'a')) (*bepa)+='a'-'A';
    bepa++;
  }
  return apa;
}

/* Append "word " to dest, refusing to overflow the destination buffer. */
static int append_word_with_space(char *dest, size_t dest_size, char *word)
{
  size_t dlen, wlen;

  dlen = strlen(dest);
  wlen = strlen(word);
  if (dlen + wlen + 1 >= dest_size)
    return 0;
  memcpy(dest + dlen, word, wlen);
  dest[dlen + wlen] = ' ';
  dest[dlen + wlen + 1] = '\0';
  return 1;
}

/* Add all objects in the inventory of 'first' and itself to the list 'parent'
   antal holds the number of elements in the 'parent' list.
*/
void fixlist(OBJECT *first, struct altern_objects **parent, int *antal) {
  OBJECT *cur;
  struct altern_objects *this,*tail;
  cur=first; this=*parent;
  if (!cur) return;
  while (cur) {
    if (cur->contains) fixlist(cur->contains,&this,antal);
    tail=this;
    this=(struct altern_objects *) xalloc(sizeof(struct altern_objects));
    this->ao_obj=cur; (*antal)++;
    add_ref(cur,"parse->fixlist");
    if (gDebug) fprintf(stderr," Obj:%s ,",cur->name);
    this->next=tail; cur=cur->next_inv;
  }
  *parent=this;
}

/* Create a list of the theoretically accessible objects from 'src'
*/
int makeobjlist(struct altern_objects **alist, OBJECT *src) {
  OBJECT *env,*cur;
  struct altern_objects *this;
  int cnt;
  *alist=0;
  if (!src) env=command_giver->super;
  else env=src;
  if (!env) return 0;
  if (gDebug) fprintf(stderr,"Make list: %s\n",env->name);
  if (!(env->contains)) return 0;
  cur=env->contains;
  this=0; cnt=0;
  fixlist(cur,&this,&cnt); *alist=this;
  return cnt;
}

/* Routines to use for alternate filters
*/

/* Return number of objects in the list
*/
int itnumalt()
{ 
  int ant;
  struct altern_objects *ao;

  ant=0; ao=gPobjects;
  while (ao) {
    ant++;
    ao=ao->next;
  }
  return ant;
}

/* Return a pointer to the numbered element of the list
*/ 
OBJECT *italt(int a)
{ 
  int ant;
  struct altern_objects *ao;

  ant=0; ao=gPobjects;
  while (ao) {
    ant++;
    if (ant==a) return ao->ao_obj;
    ao=ao->next;
  }
  return 0;
}

/* Free the list, parameter bas is obsolete
*/
void italt_new(int bas) {
  (void)bas;

  /* Free p} altern_objects listan */
  struct altern_objects *ao;
  struct altern_objects *ao2;

  if (gDebug) fprintf(stderr,"Clearing italt\n");
  ao2=ao=gPobjects; gPobjects=0;
  while (ao) {
    ao=ao->next;
    free_object(ao2->ao_obj,"parse->italt_new()");
    free((char *)ao2); 
    ao2=ao;
  }
}

/* Create the list as all the theoretically accessible objects
*/
void italt_loadall(void) {
  if (gPobjects) italt_new(0);
  makeobjlist(&gPobjects,0);
}

/* Put an object at the end of the list
*/
void italt_put(OBJECT *obj) {
  struct altern_objects *ao,*old;

  if (gDebug) fprintf(stderr,"Putting: %s",obj->name);
  ao=gPobjects; old=ao;
  while (ao) {
    old=ao; ao=ao->next;
  }
  ao=(struct altern_objects *) xalloc(sizeof(struct altern_objects));
  if (old) old->next=ao; 
  else gPobjects=ao;
  ao->ao_obj=obj; ao->next=0;
  add_ref(obj,"parse->italt_put()");
  if (gDebug) fprintf(stderr,"..done\n");
}


/* Put first word of cmd in gFword and point to next word
*/
char *getfirst(char **cmd)
{
  int pos,inqoute;
  char ch,*st;

  st=*cmd; strcpy(gFword,""); if (st[0]==0) return gFword;
  ch=' '; pos=0; inqoute=0; 
  while ((st[0]<=' ') && (st[0])) st++; /* Skip leading spaces */
  if ((st[0]=='\'') || (st[0]=='[')) {
    inqoute=1; ch=st[0]; gFword[0]=st[0]; pos=1; st++;
  }
  if (ch=='[') ch=']';
  while ((pos<(KLUDGELEN-1)) && (st[0]) && (st[0]!=ch)) {
    gFword[pos++]=st[0]; st++;
    if ((st[0]) && (st[0]<' ')) st[0]=' ';  /* Replace ctrl chars */
  }
  if ((inqoute) && (pos<(KLUDGELEN-1))) {
    gFword[pos++]=ch; if (st[0]) st++;
  }
  gFword[pos]=0;  /* fprintf(stderr,"{%s}",gFword); */
  *cmd=st;
  return gFword;
}

/* Put first word of cmd in gFword
   without incrementing the callers pointer in the command string
*/
char *lookfirst(char *cmd)
{
  return getfirst(&cmd);
}     

/* Call object function in LPC
*/
int call_obj(char *fnamn, OBJECT *on, char *apa)
{
  VALUE *ret;
  
  if (gDebug) fprintf(stderr,"Calling %s in %s with %s\n",fnamn,on->name,apa);
  ret = apply(fnamn,on,make_string(apa)); 
  if (!ret) return 0;
  if ( ret->type == T_NUMBER) return ret->u.number;
  return 0;
}

/* Change a noun in pluralform to a noun in singularform
*/
char *singfix(char *str)
{
  static char sing[KLUDGELEN];
  char *sp;
  int sl;

  sl=strlen(str); if (sl<2) return str;
  sp=&str[sl-3]; strcpy(sing,str); sing[sl-3]=0;
  if (gDebug) fprintf(stderr,"To singular (%s) end(%s) begin(%s)\n",str,sp,sing);
  if (EQ(str,"corpses")) return "corpse";
  if (EQ(sp,"ses")) return strcat(sing,"s");
  else if (EQ(sp,"xes")) return strcat(sing,"x");
  else if (EQ(sp,"hes")) return strcat(sing,"h");
  else if (EQ(sp,"ies")) return strcat(sing,"y");
  else if (EQ(sp,"ves")) return strcat(sing,"fe");
  else if (sp[2]=='s') {
    strcat(sing,sp); sing[sl-1]=0;
    return sing;
  }
  if (EQ(str,"teeth")) return "tooth";
  if (EQ(str,"feet")) return "foot";
  if (EQ(str,"men")) return "man";
  if (EQ(str,"women")) return "woman";
  if (EQ(str,"child")) return "children";
  if (EQ(str,"sheep")) return "sheep";
  return str;
}
    
/* Checks with obj in gOblist to see if it accepts all gAdjectives in adjs
   Used when name of object not known, i.e cmds like: 'get all the red ones'
   if func QADJFUNC doesn't exist in obj last word of QSHORTFUNC is used
   as object name and set as suffix to the adjective in call to id()
*/
int matchadjective(char *adjs)
{

  char ad[KLUDGELEN],tot[2*KLUDGELEN],*sp,*sp2;
  struct altern_objects *ao;
  OBJECT *on;
  VALUE *ret;

  ao=gOblist;
  while (ao) {
    on=ao->ao_obj;
    sp=adjs;
    if (!sp) {
      ao=ao->next;
      continue;
    }
    while ((on) && (sp) && (*sp)) {
      while ((sp) && (*sp==' ')) sp++;
      if (!sp || !*sp) break;
      sp2=strchr(sp,' ');  
      if (sp2) {
	*sp2=0;
	strncpy(ad,sp,sizeof ad - 1); ad[sizeof ad - 1] = '\0'; *sp2=' ';
	if (!call_obj(QADJFUNC,on,ad)) { /* Not ok */
	  ret = apply(QSHORTFUNC,on,&const0);
	  if (ret->type==T_STRING) {
	    sp=ret->u.string; sp2=backstrchr(sp,' ');
	    if (sp2) sp=sp2;
	    (void)snprintf(tot,sizeof tot,"%s %s",ad,sp); lowercase(tot);
	    if (!call_obj(QIDFUNC,on,tot)) on=0;
	  }
	  else on=0;
	}
      }
      sp=sp2;
    }
    if (on) italt_put(on);
    ao=ao->next;
  }
  return itnumalt();
}

/* Decide if a word is a general word of type: those, ones
*/
int check_for_general(char *onam, int plur)
{

  static char* plurpron[] = {"those","them","these","ones","$"};
  static char* singpron[] = {"this","one","it","$"};
  int ilop;
  
  if (EMPTY(onam)) return 0;
  if (plur) {
    ilop=0; 
    while (!EQ(plurpron[ilop],"$")) if (EQ(plurpron[ilop++],onam)) return 1;
  }
  else {
    ilop=0; 
    while (!EQ(singpron[ilop],"$")) if (EQ(singpron[ilop++],onam)) return 1;
  }
  return 0;
}

int order_num(char *wd) { /* Only positive numbers and zero */

  static char *onums[] = {"first","second","third","fourth","fifth","sixth",
		  "seventh","eighth","nineth","tenth",
		  "eleventh","twelfth","thirteenth","fourteenth","fifteenth","sixteenth",
		  "seventeenth","eighteenth","nineteenth","dummy"};
  static char *onumt[] = {"twenty","thirty","forty","fifty","sixty","seventy",
		  "eighty","ninety"};
  static char *onumta[] = {"twentieth","thirtieth","fortieth","fiftieth","sixtieth","seventieth",
		  "eightieth","ninetieth"};

  char ns[KLUDGELEN];

  int ilop,nm;

  if (EMPTY(wd)) return -1;
  for (ilop=1;ilop<20;ilop++) {
    if (EQ(onums[ilop-1],wd)) return ilop;
  }
  for (nm=0;nm<8;nm++) for (ilop=0;ilop<9;ilop++) {
    if (ilop>0) sprintf(ns,"%s%s",onumt[nm],onums[ilop-1]);
    else strcpy(ns,onumta[nm]);
    if (EQ(ns,wd)) return 20+nm*10+(ilop);
  }
  return -1;
}

int numeric(char *wd) { /* Only positive numbers and zero */

  static char *nums[] = {"one","two","three","four","five","six",
		  "seven","eight","nine","ten",
		  "eleven","twelve","thirteen","fourteen","fifteen","sixteen",
		  "seventeen","eighteen","nineteen"};
  static char *numt[] = {"twenty","thirty","forty","fifty","sixty","seventy",
		  "eighty","ninety"};

  char ns[KLUDGELEN];

  int ilop,nm;

  if (EMPTY(wd)) return -1;
  if (sscanf(wd,"%d",&nm)) return (nm>=0) ? nm : -1;
  if ((EQ(wd,"a")) || (EQ(wd,"an"))) return 1;
  for (ilop=1;ilop<20;ilop++) if (EQ(nums[ilop-1],wd)) return ilop;
  for (nm=0;nm<8;nm++) for (ilop=0;ilop<9;ilop++) {
    if (ilop>0) sprintf(ns,"%s%s",numt[nm],nums[ilop-1]);
    else strcpy(ns,numt[nm]);
    if (EQ(ns,wd)) return 20+nm*10+(ilop);
  }
  return -1;
}

/* Searches commandstring for "adj1 adj2 ... adjN objectname"
   Stores words before name in string: gAdjective, as "adj1 adj2 adj3 "
*/
OBJECT *matchobject2(char **cmd, int plur)
{ 

  OBJECT *on;
  struct altern_objects *ao;
  char *st,*ocmd,totname[2*KLUDGELEN],tot2[2*KLUDGELEN];

  ocmd=*cmd; strcpy(gAdjective,"");
  st=lowercase(getfirst(cmd)); 
  while (st[0]) {
    if (check_for_general(st,plur)) {
      if (matchadjective(gAdjective)) return italt(1);
    }
    (void)snprintf(totname,sizeof totname,"%s%s",gAdjective,st);
    if (plur)
      (void)snprintf(tot2,sizeof tot2,"%s%s",gAdjective,singfix(st));
    ao=gOblist; 
    while (ao) {
      on=0;
      if (plur) {
	if (call_obj(QPLURIDFUNC,ao->ao_obj,totname)) on=ao->ao_obj;
	else if (call_obj(QIDFUNC,ao->ao_obj,tot2)) on=ao->ao_obj;
      }
      else if (call_obj(QIDFUNC,ao->ao_obj,totname)) on=ao->ao_obj;
      if (on) italt_put(on);
      ao=ao->next;
    }
    if (!itnumalt()) {
      if (!append_word_with_space(gAdjective, sizeof gAdjective, st)) {
	*cmd=ocmd;
	return 0;
      }
      st=lowercase(getfirst(cmd)); 
    }
    else st[0]=0;
  }
  if (itnumalt()) return italt(1);
  *cmd=ocmd; return 0;
}

/* Search the command for valid object description
*/
OBJECT *finditem(char **cmd, int plur)
{ 
  int nm;
  OBJECT *pn;
  char *ocmd,w1[KLUDGELEN];
  
  ocmd=*cmd; strcpy(w1,lowercase(getfirst(cmd)));
  if (gDebug) fprintf(stderr,"FO (plur=%d): %s\n",plur,ocmd);
  italt_new(0); /* Clear alternate list */
  gWantnum=1; /* Antal objekt som anv{ndaren vill ta */
  if (EMPTY(w1)) {
    *cmd=ocmd;
    return 0;
  }

  /* Must be hardcode skipped for recursive with plural
   */
  if (EQ("the",w1)) {
    if (gDebug) fprintf(stderr,"Skip 'the'\n");
    *cmd=ocmd; getfirst(cmd); 
    if ((pn=finditem(cmd,plur))) return pn;
    else { *cmd=ocmd; return 0; }
  }
  
  /* Check things like: some <objname>
   */
  else if ((EQ("some",w1)) && (plur==0)) {
    *cmd=ocmd; getfirst(cmd);
    /* Skip of in : some of the bottles */
    if (EQ(lowercase(lookfirst(*cmd)),"of")) getfirst(cmd);

/* Patch by: -- nancy mintz (201) 522-6095 attunix!nlm, nlm@attunix.att.com
*/

#ifdef RANDOM
       nm=2+random()  % 6; /* some objects is random:2-7 */
#else /* RANDOM */
#ifdef DRAND48
       nm=2+(drand48()  * 6); /* some objects is random:2-7 */
#else /* DRAND48 */
       nm=2+time(0l)  % 6; /* some objects is random:2-7 */
#endif /* DRAND48 */
#endif /* RANDOM */

    if (gDebug) fprintf(stderr,"some rnd = %d\n",nm);
    if ((pn=finditem(cmd,1))) { 
      gWantnum=nm; 
      return pn; 
    }
  }
  
  /* Check things like: three <objname>
   */
  else if ( (plur==0) && ((nm=numeric(w1))>0) ) {
    if (gDebug) fprintf(stderr,"Numeral: %d (%s)\n",nm,w1);
    *cmd=ocmd; getfirst(cmd);
    /* Skip of in : two of the bottles */
    if (EQ(lowercase(lookfirst(*cmd)),"of")) getfirst(cmd);
    if ((pn=finditem(cmd,nm-1))) {
      gWantnum=nm; return pn;
    }
  }
  
  /* Check things like: third <objname>
   */
  else if ( (plur==0) && ((nm=order_num(w1))>0) ) {
    if (gDebug) fprintf(stderr,"Order: %d (%s)\n",nm,w1);
    *cmd=ocmd; getfirst(cmd);
    /* Skip of in : second of the bottles */
    if (EQ(lowercase(lookfirst(*cmd)),"of")) {
      getfirst(cmd);
      if ((pn=finditem(cmd,1))) { /* Fix second of the bottles */
	gWantnum=-nm; return pn;
      }
    }
    else {
      if ((pn=finditem(cmd,0))) { /* Fix second bottle */
	gWantnum=-nm; return pn;
      }
    }
  }
  
  
  /* Check things like: all <objname>
   */
  else if ((EQ("all",w1)) && (plur==0)) {
    if (gDebug) fprintf(stderr,"All found\n");
    *cmd=ocmd; getfirst(cmd);
    /* Skip of in : all of the bottles */
    if (EQ(lowercase(lookfirst(*cmd)),"of")) { getfirst(cmd); }
    if ((pn=finditem(cmd,1))) { /* Handle: get all apples */
      gWantnum=0; /* Antal objekt som anvandaren vill ta 0 == all */
      return pn;
    }
    else { /* Handle: get all */
      italt_loadall(); pn=(OBJECT *) italt(1); 
      gWantnum=0; /* Antal objekt som anvandaren vill ta 0 == all */
    }
  }
  
  /* Search for: adj1 adj2 ... adjN objectname
   */
  else {
    *cmd=ocmd; if (gDebug) fprintf(stderr,"Objtry: %s\n",ocmd);
    if (plur) pn=matchobject2(cmd,plur);
    else { /* Standard singular */
      pn=matchobject2(cmd,0); 
      if (!pn) { /* Handle things of type: get apples */
	pn=matchobject2(cmd,1); 
	if (pn) gWantnum = 0; /* Default this to all */
      }
    }
    if (gDebug) fprintf(stderr,"Done matchobject in finditem.\n");
  }
  
  if (pn == 0) *cmd=ocmd;
  if (gDebug) fprintf(stderr,"Done finditem.\n");
  return pn;
}

/* Find match to a %i in pattern
*/
int findobject(char **cmd)
{
  int nm,s;
  OBJECT *ob;
  
  if (finditem(cmd,0)) {
    if (gCarg) {
      VALUE *v;
      VECTOR *p;
      if (gDebug) fprintf(stderr,"Assign array\n");
      nm=itnumalt();
      v=(VALUE *)allocate_array(nm+1);
      p=v->u.vec;
      assign_svalue(&p->item[0],make_number(gWantnum));

      /* Make array in reverse order from italt() because
         makeobjlist() has reversed the order on entry
      */
      for (s=1;s<=nm;s++) {
	p->item[s].type = T_OBJECT;
	p->item[s].u.ob = ob = italt(nm+1-s);
	if (!ob) if (gDebug) fprintf(stderr,"No object from italt %d\n",s);
	if (ob) add_ref(ob,"parse_command");
      }
      assign(gCarg,v); /* Assign value to the current argument */

      if (gDebug) fprintf(stderr,"Assign array2\n");
    }
    return 1;
  }
  return 0;
}

/* Find match to a %l in pattern
*/
OBJECT *findplay(char **cmd)
{ 
  OBJECT *pn;
  char w1[KLUDGELEN];

  strcpy(w1,lowercase(lookfirst(*cmd)));
  /* can be fixed later with call to LPC command_giver->query_real_name() 
  if (EQ(w1,"me")) strcpy(w1,getmyname_jp());
  if (EQ(w1,"myself")) strcpy(w1,getmyname_jp());
  */
  pn=(OBJECT *) find_living_object(w1, 1);  /* Find player by name */
  if (pn) {
    getfirst(cmd); 
    if (gCarg) {
      VALUE *v;
      v=alloc_value();
      v->type = T_OBJECT;
      v->u.ob = pn;
      add_ref(pn,"parse->findplay()");
      assign(gCarg,v);
    }
  }
  return pn;
}

/* Find match to %p in pattern when a list of words has been supplied
*/
int findword(char **cmd, VALUE *v)
{
  char *w;
  VECTOR *p;
  struct svalue sv;
  int cnt,m,f;

  w = lookfirst(*cmd); lowercase(w); p=v->u.vec;
  cnt=0; m=p->size; f=-1;
  while (cnt<m) {
    if (p->item[cnt].type == T_STRING) {
      if (strcmp(p->item[cnt].u.string,w)==0) {
	f=cnt; cnt=m;
      }
    }
    cnt+=1;
  }
  if (f<0) return 0;
  getfirst(cmd);
  if (!f) return 1; /* Match and word in first element */
  /*
    Swap element 0 and f in array
  */
  sv=p->item[0]; p->item[0]=p->item[f]; p->item[f]=sv;
  return 1;
}

/* Find match to %p in pattern
*/
int findprepos(char **cmd)
{
  char *w;
  static char *hard_prep[] = {"on","in","under","from","behind","beside",0};
  VALUE *v;
  int cnt;

  if (gForprepos) {
    v = (VALUE *) get_one_expr(&gForprepos, "parse");
    if ((v != 0) && (v->type == T_POINTER)) return findword(cmd,v);
  }

  /* No wordlist sent, use hard coded prepositions and return a string
  */
  w = lookfirst(*cmd); lowercase(w);
  cnt=0; while (hard_prep[cnt]) {
    if (strcmp(w,hard_prep[cnt])==0) {
      getfirst(cmd); /* Skip this word */
      if (gCarg) {
	assign(gCarg,make_string(w)); return 1;
      }
    }
    cnt++;
  }
  return 0;
}

/* Find match to %o in pattern
*/
int findsingle(char **cmd)
{
  if (finditem(cmd,0)) {
    if ((itnumalt()==1) && (gCarg)) {
      VALUE v;
      v.type=T_OBJECT;
      v.u.ob=italt(1);
      assign(gCarg,&v); /* Assign value to the current argument */
    }
    return 1;
  }
  return 0;
}

/* Get the first parsetype of the pattern
*/
int get1ps(char **parse, LNODE **lin, int skip)
{
  char *cod,ch;
  int pt;
  LNODE *l;

  pt=EP; cod=getfirst(parse); l=*lin;
  ch=cod[0];
  if (ch=='%')
    ch=(char)tolower((unsigned char)cod[1]);
  switch (ch) {
  case 'i':pt=IT; break;
  case 'l':pt=US; break;
  case 's':pt=TX; break;
  case 'w':pt=W1; break;
  case 'o':pt=SI; break;
  case 'p':pt=PP; break;
  case 'd':pt=NUM; break;
  case '\'':sscanf(cod,"'%[^\']",gMword); pt=DTX; break;
  case '[':sscanf(cod,"[%[^]]",gMword); pt=OTX;
    if (gMword[strlen(gMword)-1]==']') gMword[strlen(gMword)-1]=0;
    break;
  case '/':pt=ALT; break;
  }
  if (gDebug) fprintf(stderr,"(%s,%s,%d,%c)",cod,gMword,pt,ch);
  gCarg=0;
  if ((skip) || (pt==DTX) || (pt==OTX) || (pt==ALT)) return pt;
  if (l && l->a1) { gForprepos=l; gCarg=l->a1; l=l->a2; }
  else { gCarg = 0; gForprepos=0; }
  *lin=l;
  return pt;
}

/* Add word to the string that matches the latest %s
*/
void addword(char *d, char *s)
{
  if ((strlen(d)+strlen(s))<(KLUDGELEN-2)) {
    if (!EMPTY(d)) strcat(d," "); 
    strcat(d,s);
  }
  if (gTxarg) assign(gTxarg,make_string(d));
}

/* Main function, called from interpret.c
*/
VALUE *parse(LNODE *l)
{
  /* Arguments:
     string      Command
     ob/arr      List of accessible objects or recurse start object
     pattern     parsepattern
     destargs
     */
  
  int altflag,         /* =1 (true) i altkedja, match har intr{ffat */
  txflag,              /* = 1 om vi adderar text till str{ng */
  ptyp;                /* Parsetype %o %i %l %p %s 'word' [word] / %w */
  
  char *parse;             /* Parsepattern */
  char *cmd;               /* Command to be parsed */
  VALUE *p;                /* temp */
  char *ps,*cs;            /* Temporary parse and command */
  char *ops,*ocs;          /* Temporary parse and command */
  char tx_jp[KLUDGELEN];   /* Fill up string for %s */
  int new_routines(char *r, LNODE *l);

  gDebug = 0;
  if (gDebug) {
    fprintf(stderr,"Hold on to your toes, entering JnAparse. ");
  }
  /* First get the command
   */
  p = (VALUE *) get_one_expr(&l, "parse");
  if (gDebug) fprintf(stderr,"1");
  if (p == 0 || p->type != T_STRING)
    error("Bad first argument to parse_command\n.");
  cs = p->u.string;
  if (cs == 0) return &const0;

  /* Here we make a GIANT kludge to test new routines
  */
  if (cs[0]=='@') return make_number(new_routines(&cs[1],l));

  ocs = cmd = (char *) string_copy(cs);
  
  /* Then get the accessible objects
   */
  p = (VALUE *) get_one_expr(&l, "parse");
  if (gDebug) fprintf(stderr,"2");
  if (p == 0 || ((p->type != T_OBJECT) && (p->type != T_POINTER)))
    error("Bad second argument to parse_command\n.");
  if (p->type == T_OBJECT) makeobjlist(&gOblist,p->u.ob);
  else { /* Copy list of given accessible objects in reverse order */
    VECTOR *v;
    SVALUE *vv;
    int cnt;
    v=p->u.vec; italt_new(0);
    for (cnt=0;cnt<v->size;cnt++) {
      vv=&(v->item[v->size-1-cnt]);
      if (vv->type==T_OBJECT) italt_put(vv->u.ob);
    }
    gOblist=gPobjects; gPobjects=0;
  }
  
  /* Then get the parse pattern
   */
  p = (VALUE *) get_one_expr(&l, "parse");
  if (gDebug) fprintf(stderr,"3");
  if (p == 0 || p->type != T_STRING)
    error("Bad third argument to parse_command\n.");
  ps = p->u.string;
  ops = parse = (char *) string_copy(ps);
  gPobjects=gOblist;
  if (gDebug) fprintf(stderr,"Has made list of Acsobjects. %d \n",itnumalt());
  if (gDebug) fprintf(stderr,"Parsing: %s, with pattern: %s\n",cmd,parse);
  gPobjects=0;

  /* Start parsing
   */
  txflag=-1; gCarg=0; gTxarg=0;
  
  ptyp=get1ps(&parse,&l,0); /* Get first pattern from parse */

  while (ptyp != EP) {
    if (gDebug) fprintf(stderr,"Pattern: %d\n",ptyp);
    altflag=0;
    switch(ptyp) { /* See which pattern type to search for */

    case EP: break;
      
    case SI: if (findsingle(&cmd)) altflag=1; /* %o */ break;
    case IT: if (findobject(&cmd)) altflag=1; /* %i */ break;
    case US: if (findplay(&cmd)) altflag=1;   /* %l */ break;
    case PP: if (findprepos(&cmd)) altflag=1; /* %p */ break;

    case TX: /* %s */
      txflag=1; strcpy(tx_jp,""); gTxarg=gCarg; altflag=1; 
      break;
      
    case DTX: /* 'word' */
      if (EQ(lowercase(gMword),lowercase(lookfirst(cmd)))) {
	getfirst(&cmd); altflag=1;
      }
      break;

    case OTX: /* [word] */
      if (EQ(lowercase(gMword),lowercase(lookfirst(cmd)))) {
	getfirst(&cmd); 
      }
      altflag=1; /* Always take next parsepattern type [word] is opt */
      break;

    case ALT: /* / */
      /* FALLTHROUGH */
    case W1: /* %w */
      gTxarg=gCarg; strcpy(tx_jp,""); altflag=1;
      addword(tx_jp,getfirst(&cmd));
      break;

    case NUM: /* %d */
      if ((altflag=numeric(lookfirst(cmd)))>0) {
	if (gCarg) assign(gCarg,make_number(altflag));
	altflag=1; getfirst(&cmd);
      }
      break;
    }

    /* Pattern checked altflag==1 indicates match
    */
    if (altflag) { /* Pattern matched, fetch next and end string input */
      if (ptyp != TX) txflag=-1; /* End string input if not just started */
      ptyp=get1ps(&parse,&l,0);
      while (ptyp == ALT) { /* Skip left over alternatives */
	(void)get1ps(&parse,&l,1); /* Skip this pattern */
	ptyp=get1ps(&parse,&l,0); /* Next real pattern or ALT */
      }
    }
    else { /* Pattern did not match */
      char *a;
      LNODE *try;
      int tmp;
      a=parse; try=l;
      tmp=get1ps(&a,&try,0);
      if (tmp == ALT) { 
	(void)get1ps(&parse,&l,0); /* Skip ALT */
	ptyp=get1ps(&parse,&l,0); /* Next real pattern or ALT */
      }
      else {
	if (txflag>=0) { /* %s is defined, add word and try pattern again */
	  addword(tx_jp,getfirst(&cmd));
	}
	else break; /* Impossible to match pattern, exit */
      }
    }
    
    if (gDebug) fprintf(stderr,"Pattern, after: %d\n",ptyp);
  } 

  /* End of pattern reached, what have got? What is left?
  */
  if (gDebug) fprintf(stderr,"Pattern ended\n");

  if (txflag>=0) { /* It ended with a %s, fill up the string */
    while (!EMPTY(cmd)) addword(tx_jp,getfirst(&cmd));
  }

  if (!EMPTY(cmd)) ptyp=ALT; /* No match so set ptyp != EP */

  /* Now clean up our mess, no alloced mem should remain
  */
  free(ocs); free(ops);
  if (gPobjects) italt_new(0); /* Free alternate object list */
  if (gOblist) {
    gPobjects=gOblist; italt_new(0); /* Free list of accessible objects */
  }
  if (ptyp==EP) {
    if (gDebug) fprintf(stderr,"Exiting Jnaparse with match\n");
    return &const1; /* Successfull match */
  }
  else {
    if (gDebug) fprintf(stderr,"Exiting Jnaparse: No match\n");
    return &const0; /* No match */
  }
}

/*

   End of Parser

*********************************************************/

/* Runs all elements of an array through ob::func
   and returns an array holding those elements that ob::func
   returned 1 for
*/
VALUE *filter(VALUE *arr, char *func, OBJECT *ob)
{
  VECTOR *p,*r;
  VALUE *v;
  char *flags;
  int cnt,res;
  
  if ((!arr) || (arr->type != T_POINTER)) return &const0;
  p=arr->u.vec; res=0;
  if (p->size<1) return &const0;
  flags=xalloc(p->size+1); 
  for (cnt=0;cnt<p->size;cnt++) {
    flags[cnt]=0;
    v=apply(func,ob,(VALUE *) copy_svalue(&(p->item[cnt])));
    if ((v) && (v->type==T_NUMBER)) {
      if (v->u.number) { flags[cnt]=1; res++; }
    }
  }
  if (!res) return &const0;
  v = (VALUE *) allocate_array(res);
  r = v->u.vec; res=0;
  for (cnt=0;cnt<p->size;cnt++) if (flags[cnt]) {
    assign_svalue(&r->item[res],copy_svalue(&p->item[cnt]));
    res++; if (res>r->size) break;
  }
  free(flags);
  return v;
}


/* Kludge grandioso to test new routines
   Uses parse_command("@routine",...) as entry ** BLUSH **
*/
int new_routines(char *r, LNODE *l)
{
  struct value *ret = &const0;
  struct value *arg, *arg1, *arg2;
  VALUE *filter PROT((VALUE *arr,char *func,OBJECT *ob));
  VALUE *describe_items PROT((VALUE *arr,char *func,int live));

  if (EQ(r,"filter")) {
    arg = (VALUE *) get_one_expr(&l,"filter");
    arg1 = (VALUE *) get_one_expr(&l,"filter");
    arg2 = (VALUE *) get_one_expr(&l,"filter");
    if (arg == 0 || arg->type != T_POINTER)
      error("Bad first argument to filter().\n");
    if (arg1 == 0 || arg1->type != T_STRING)
      error("Bad second argument to filter().\n");
    if (arg2 == 0 || arg2->type != T_OBJECT)
      error("Bad third argument to filter().\n");
    ret = filter(arg,arg1->u.string,arg2->u.ob);
  }
  else if (EQ(r,"describe")) {
    arg = (VALUE *) get_one_expr(&l,"describe");
    arg1 = (VALUE *) get_one_expr(&l,"describe");
    arg2 = (VALUE *) get_one_expr(&l,"describe");
    if (arg == 0 || arg->type != T_POINTER)
      error("Bad first argument to describe().\n");
    if (arg1 == 0 || arg1->type != T_STRING)
      error("Bad second argument to describe().\n");
    if (arg2 == 0 || arg2->type != T_NUMBER)
      error("Bad third argument to describe().\n");
    ret = (VALUE *) describe_items(arg,arg1->u.string,arg2->u.number);
  }
  else if (EQ(r,"toggle debug")) {
    if (gDebug) gDebug=0;
    else gDebug=1;
    return gDebug;
  }
  if (l && (l->a1)) {
    assign(l->a1,ret); 
    return 1;
  }
  return 0;
}


/*****************************************************

  This is the describer

*/
#define QNPCFUNC "query_npc"        /* Called in living to see if NPC */
#define QNAMEFUNC "query_name"      /* Called in living to get name */

int get_positive(SVALUE *v)
{
  if (v->type!=T_NUMBER) return 0;
  return v->u.number;
}

OBJECT *get_object(SVALUE *v)
{
  if (v->type!=T_OBJECT) return 0;
  return v->u.ob;
}


/* Makes a description string from the structured array arr
   using the objectdescription for each object given by the
   LPCfunction func in each object

   Descriptions are on the form:
     two green apples and an orange
     a glass bowl and some keys

   Prefixes and suffixes is not added.
*/

VALUE *describe_items(VALUE *arr, char *func, int live)
{
  VECTOR *p,*v1;
  OBJECT *ob;
  char res[10*KLUDGELEN],*tmp;
  int siz,num,cnt1,cnt2;
  char *fixname PROT((OBJECT *,int,char *));
  char *live_plurfix PROT((OBJECT *,int,char *));

  if (gDebug) fprintf(stderr,"Describing:");
  p=arr->u.vec;
  if (p->size<1) return make_number(0);
  if (!(siz=get_positive(&(p->item[0])))) return make_number(0);
  strcpy(res,"@@@"); cnt1=1; cnt2=siz;
  while (cnt1<=cnt2) {
    if (p->item[cnt1].type!=T_POINTER) return make_string(res);
    v1=p->item[cnt1].u.vec;
    if (v1->size<2) return make_string(res);
    num=get_positive(&v1->item[0]);
    ob=get_object(&v1->item[1]);
    if ((!num) || (!ob)) return make_string(res);
    if (live) tmp=live_plurfix(ob,num,func);
    else tmp=fixname(ob,num,func);
    while (tmp[strlen(tmp)-1]==' ') tmp[strlen(tmp)-1]=0; 
    if (gDebug) fprintf(stderr,".%s.",tmp);
    if (strlen(tmp)+strlen(res)>9*KLUDGELEN) {
      strcat(res," among other things"); return make_string(res);
    }
    if (EQ("@@@",res)) strcpy(res,"");
    if (siz == 1) strcat(res,tmp);
    else if (siz == 2) { strcat(res,tmp); strcat(res," and "); }
    else if (siz > 2) { strcat(res,tmp); strcat(res,", "); }
    cnt1++; siz--;
  }
  if (gDebug) fprintf(stderr,":\n");
  return make_string(res);
}

/* Change a noun in singularform to a noun in pluralform
*/
char *pfix(char *str)
{
  static char plur[KLUDGELEN+10];
  char *sp,*sp2;
  int sl;

  sl=strlen(str); 
  if (sl<2) return strcpy(plur,str);
  sp=&str[sl-1]; strcpy(plur,str); plur[sl-1]=0;
  sp2=&str[sl-2];
  if (gDebug) fprintf(stderr,"To plural (%s) end(%s) begin(%s)\n",str,sp,plur);
  if (EQ(sp,"s")) strcat(plur,"ses");
  else if (EQ(sp,"x")) strcat(plur,"xes");
  else if (EQ(sp,"h")) strcat(plur,"hes");
  else if (EQ(sp,"y")) strcat(plur,"ies");
  else if (EQ(sp2,"fe")) {
    strcpy(plur,str); plur[sl-2]=0; strcat(plur,"ves");
  } 
  else if (EQ(str,"tooth")) strcpy(plur,"teeth");
  else if (EQ(str,"foot")) strcpy(plur,"feet");
  else if (EQ(str,"man")) strcpy(plur,"men");
  else if (EQ(str,"woman")) strcpy(plur,"women");
  else if (EQ(str,"children")) strcpy(plur,"child");
  else if (EQ(str,"sheep")) strcpy(plur,"sheep");
  else {
    strcpy(plur,str); strcat(plur,"s");
  }
  return plur;
}

/* Add the correct article to a noun ( a or an )
*/
char *artfix(char *str)
{
  static char tmp[KLUDGELEN+3];

  if (gDebug) fprintf(stderr,".artfix.");
  if (strchr("AEIOUaeiou",str[0])) {
    strcpy(tmp,"an "); strcat(tmp,str); return tmp;
  }
  strcpy(tmp,"a "); strcat(tmp,str); return tmp;
}

/* Get the last word of the description and fix it to the right form
*/
char *splitfix(char *str, int ant)
{
  char *sp;
  static char tmp[KLUDGELEN+10];
  char tmp2[KLUDGELEN+10];

  strcpy(tmp2,str); sp=strrchr(tmp2,' ');  /* sp => last word in desc */ 
  if (gDebug) fprintf(stderr,".splitfix.");
  if (!sp) {                               /* Only 1 word in desc */
    strcpy(tmp,str); lowercase(tmp);
    if (ant>1) strcpy(tmp,pfix(tmp));
    return tmp;
  }
  else {
    *sp=0; strcpy(tmp,&sp[1]);  /* tmp2+" "+tmp is hole desc */
  }
  if (ant>1) strcpy(tmp,pfix(tmp));
  lowercase(tmp); 
  strcat(tmp2," "); strcat(tmp2,tmp); strcpy(tmp,tmp2);
  return tmp;
}

/* Add a prefix number in textform to the noun
*/
char *add_num(char *dest, char *str, int ant)
{
  strcpy(dest,"");
  if (gDebug) fprintf(stderr,".splitfix.");
  if (ant == 2) {
    strcpy(dest,"two "); strcat(dest,str);
  }
  else if (ant == 3)  {
    strcpy(dest,"three "); strcat(dest,str);
  }
  else if (ant == 4) {
    strcpy(dest,"four "); strcat(dest,str);
  } 
  else if (ant < 8) {
    strcpy(dest,"some "); strcat(dest,str);
  } 
  else  {
    strcpy(dest,"many "); strcat(dest,str);
  }
  return dest;
}

/* Fix the description into pluralform, used when no "plural_short" exists
*/
char *plurfix(char *dest, char *str, int ant)
{
  char tmp[KLUDGELEN];
  int itmp;

  if (gDebug) fprintf(stderr,".plurfix.");
  if (sscanf(str,"%d",&itmp)) {
    strcpy(dest,str); return dest;
  }
  if (ant == 1) {
    if EQN("the ",str) {
      strcpy(dest,"the "); strcat(dest,splitfix(&str[4],ant)); return dest;
    }
    if EQN("The ",str) {
      strcpy(dest,"the "); strcat(dest,splitfix(&str[4],ant)); return dest;
    }
    strcpy(dest,artfix(splitfix(str,ant)));
    return dest;
  }
  else {
    itmp=sscanf(str,"the %s",tmp);
    if (!itmp) itmp=sscanf(str,"The %s",tmp);
    if (!itmp) strcpy(tmp,str);
    return add_num(dest,splitfix(str,ant),ant);
  }
}

/* Fix the descriptiom of a 'dead' object
*/
char *fixname(OBJECT *ob, int ant, char *func)
{
  char *str,*name,tmp[KLUDGELEN];
  static char retval[2*KLUDGELEN];
  VALUE *v;
  
  if (gDebug) fprintf(stderr,".fixname.");
  v=apply(func,ob,0);
  if ((v) && (v->type==T_STRING)) str=v->u.string;
  else str="shadow thing";
  if (ant>1) {
    (void)snprintf(tmp,sizeof tmp,"plural_%s",func);
    v=apply(tmp,ob,0);
    if ((v) && (v->type==T_STRING)) name=v->u.string;
    else name=0;
    if (name) {
      strncpy(tmp,name,KLUDGELEN-1); tmp[KLUDGELEN-1]=0; /* Cry: Whaaaaah */
      return add_num(retval,tmp,ant);
    }
  }
  strncpy(tmp,str,KLUDGELEN-1); tmp[KLUDGELEN-1]=0; /* Cry: Whaaaaah */
  if EQN("a ",tmp) return plurfix(retval,&tmp[2],ant);
  else if EQN("an ",tmp) return plurfix(retval,&tmp[3],ant);
  else if EQN("A ",tmp) return plurfix(retval,&tmp[2],ant);
  else if EQN("An ",tmp) return plurfix(retval,&tmp[3],ant);
  else if EQN("AN ",tmp) return plurfix(retval,&tmp[3],ant);
  else if EQN("aN ",tmp) return plurfix(retval,&tmp[3],ant);
  return plurfix(retval,tmp,ant);
}

/* Fix the description of a living thing
*/
char *live_plurfix(OBJECT *ob, int ant, char *func)
{
  char *str,*name,tmp[KLUDGELEN];
  static char retval[2*KLUDGELEN];
  VALUE *v;
  
  if (gDebug) fprintf(stderr,".live_plurfix.");
  v=apply(QNPCFUNC,ob,0);
  if ((v) && (v->type==T_NUMBER) && (v->u.number)) return fixname(ob,ant,func);
  if (ant==1) {
    v=apply(QNAMEFUNC,ob,0);
    if ((v) && (v->type==T_STRING)) str=v->u.string;
    else {
      v=apply(func,ob,0);
      if ((v) && (v->type==T_STRING)) str=v->u.string;
      else {
	strcpy(tmp,"shadow"); str=tmp;
      }
    }
    strncpy(retval,str,2*KLUDGELEN-1); retval[2*KLUDGELEN-1]=0; 
    return retval;
  }
  else {
    v=apply(QNAMEFUNC,ob,0);
    if ((v) && (v->type==T_STRING)) str=v->u.string;
    else {
      v=apply(func,ob,0);
      if ((v) && (v->type==T_STRING)) str=v->u.string;
	else str="shadow";
    }
    (void)snprintf(tmp,sizeof tmp,"plural_%s",QNAMEFUNC);
    v=apply(tmp,ob,0);
    if ((v) && (v->type==T_STRING)) name=v->u.string;
    else {
      (void)snprintf(tmp,sizeof tmp,"plural_%s",func);
      v=apply(tmp,ob,0);
      if ((v) && (v->type==T_STRING)) name=v->u.string;
      else name=0;
    }
    if (name) {
      strncpy(tmp,name,KLUDGELEN-1); tmp[KLUDGELEN-1]=0; /* Cry: Whaaaaah */
      return add_num(retval,tmp,ant);
    }
    strncpy(tmp,str,KLUDGELEN-1); tmp[KLUDGELEN-1]=0; /* Cry: Whaaaaah */
    return plurfix(retval,tmp,ant);
  }
}
/*
  End of describer
**************************************************/


/*
  Other useful routines, Please could these be added Lars?
*/

/* Concatenation of two arrays into one
*/
VALUE *add_array(VALUE *arr1, VALUE *arr2)
{
  VECTOR *p,*r,*d;
  VALUE *v;
  int cnt,res;
  
  if ((!arr1) || (arr1->type != T_POINTER)) return &const0;
  if ((!arr2) || (arr2->type != T_POINTER)) return &const0;
  p=arr1->u.vec;
  r=arr2->u.vec;
  if (p->size+r->size<1) return &const0;
  
  v = (VALUE *) allocate_array(p->size+r->size);
  d = v->u.vec; res=0;
  for (cnt=0;cnt<p->size;cnt++) {
    assign_svalue(&d->item[res],copy_svalue(&p->item[cnt]));
    res++; 
  }
  for (cnt=0;cnt<r->size;cnt++) {
    assign_svalue(&d->item[res],copy_svalue(&r->item[cnt]));
    res++; 
  }
  return v;
}

/* Deletion of one element from an array
*/
VALUE *del_element(VALUE *arr1, VALUE *elem)
{
  VECTOR *p,*d;
  VALUE *v;
  int cnt,res,res2;
  
  if ((!arr1) || (arr1->type != T_POINTER)) return &const0;
  if ((!elem) || (elem->type != T_NUMBER)) return &const0;
  p=arr1->u.vec; 
  res=elem->u.number;
  if (p->size+1<res) return &const0;
  
  v = (VALUE *) allocate_array(p->size-1);
  d = v->u.vec; res2=0;
  for (cnt=0;cnt<p->size;cnt++) {
    if (res!=cnt) {
      assign_svalue(&d->item[res2],copy_svalue(&p->item[cnt]));
      res2++; 
    }
  }
  return v;
}
