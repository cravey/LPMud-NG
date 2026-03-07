#include "std.h"
#undef EXTRA_RESET
object mechanic;
#define EXTRA_RESET\
   if (!mechanic || !living(mechanic)) {\
       object beer;\
       object wrench;\
       mechanic = clone_object("obj/monster");\
       call_other(mechanic, "set_name", "mechanic");\
       call_other(mechanic, "set_level", 12);\
       call_other(mechanic, "set_al", 200);\
      call_other(mechanic, "set_short", "A huge dirty mechanic is here");\
    call_other(mechanic, "set_wc", 15);\
    call_other(mechanic, "set_aggressive", 1);\
     beer = clone_object("obj/beer");\
     move_object(beer, mechanic);\
     move_object(mechanic,  this_object());\
     wrench = clone_object("obj/weapon");\
    call_other(wrench, "set_name", "wrench");\
    call_other(wrench, "set_value",100);\
   call_other(wrench, "set_class", 12);\
    call_other(wrench, "wield", "wrench");\
    call_other(wrench, "wield", "monkey wrench");\
    move_object(wrench, mechanic);\
      }


TWO_EXIT("players/kantele/wagon_stall" , "down",
         "players/kantele/dirty_sleepingroom" , "east",
         "A dirty living room",
         "It looks like a very dirty unkeept human male lives here.\n" +
         "A stairway leads down, and a door further in.\n",1)
