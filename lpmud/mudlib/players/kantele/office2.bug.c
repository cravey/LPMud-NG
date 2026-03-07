#include "std.h"

int open;


#undef EXTRA_INIT
#define EXTRA_INIT add_action("open_drawer"); add_verb("open");

#undef EXTRA_LONG
#define EXTRA_LONG\
    if (arg == "desk") {\
       write("You can see a massive oak desk with one drawer, it looks very\n");\
       write("heavy. ");\
       if (open ==1)\
          write("The drawer is open.\n");\
       if (open ==0) {\
          write("The drawer is closed.\n");\
       return 1;\
       }\
   }

#undef EXTRA_RESET
#define EXTRA_RESET\
        extra_reset();\
        open =0;\

object drawer, desk;

extra_reset() {
      }

TWO_EXIT("players/kantele/lab","north",
         "players/kantele/knights_room","east",
         "Kantele's workroom",
         "You are in Kanteles workroom, a large oak desk covers most of the\n" +
         "room, you can go north into the lab or east into a small room.\n" +
        "you can se a massive oak desk against the wall.\n",1)


open_drawer(arg) {
     if (open ==1 && arg =="drawer")  {
        write("The drawer is already open.\n");
        return 1;
        }
     if (open ==0 && arg =="drawer")  {
        write("you open the drawer.\n");
        say(call_other(this_player(), "query_name") + " opens the drawer.\n");
        open =1;
        return 1;
        }
}

close_drawer(arg) {
      if (open ==0 && arg ="drawer")  {
        write("The drawer is already closed.\n");
        return 1;
        }
      if (open ==1 && arg ="drawer")  {
        write("You close the drawer.\n");
        say(call_other(this_player(), "query_name") + " closes the drawer.\n");
        open =0;
        return 1;
        }
id(str) {
return str == "drawer" || str =="desk";
}
