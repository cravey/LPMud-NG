#include "std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
    add_action("enter"); add_verb("enter");\
    add_action("enter"); add_verb("exit");

TWO_EXIT("players/crimson/landing", "up",
         "players/crimson/stairs2", "down",
          "Rickety Old Stairs",
         "You are on a staircase of some antiquity. The steps lead down\n" +
         "into darkness and uncertainty.  It is possible to enter the clock\n"+
         "and return the landing from here. It is also possible to continue\n"+
         "up the darkened staircase.\n", 1)

enter(str) {
   if (str == "clock" || str == "through clock" || str == "grandfather clock" || str == "through grandfather clock") 
   call_other(this_player(), "move_player", "through the clock#players/crimson/landing");
   return 1;
}
