#include "std.h"

object demon;
#undef EXTRA_RESET
#define EXTRA_RESET\
    if (!demon || !living(demon)) {\
        demon = clone_object("obj/monster");\
        call_other(demon, "set_level", 15);\
        call_other(demon, "set_name", "demon");\
        call_other(demon,"set_level",12);\
        call_other(demon, "set_short", "A household demon is here");\
        call_other(demon, "set_wc", 20);\
       call_other(demon, "set_move_at_reset");\
        call_other(demon, "set_ac", 2);\
       call_other(demon, "set_aggressive", 1);\
       move_object(demon, this_object());\
    }

TWO_EXIT("players/kantele/inner_hall","west",
         "players/kantele/bar","south",
         "A tastful living room",
         "A tasteful living room, an exit leads west into the hall, you\n"  +
         "can also leave south into the bar.\n",1)
