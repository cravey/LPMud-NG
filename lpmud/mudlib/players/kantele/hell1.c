#include "std.h"

object manes;
#undef EXTRA_RESET
#define EXTRA_RESET\
        int i;\
    if (!manes || !living(manes)) {\
        i = 4;\
        while(i>0) {\
            i -= 1;\
            manes = clone_object("obj/monster");\
            call_other(manes, "set_name", "manes");\
            call_other(manes, "set_alias", "devil");\
            call_other(manes, "set_level",8);\
            call_other(manes, "set_short", "A huge mane is here");\
           call_other(manes, "set_long", "A mean hungry mane is here\n");\
            call_other(manes, "set_wc",12);\
            call_other(manes, "set_aggressive", 1);\
            call_other(manes, "set_move_at_reset");\
            call_other(manes, "set_move_at_reset");\
            move_object(manes, this_object());\
        }\
    }
FOUR_EXIT("players/kantele/hell2.c","north",
           "players/kantele/hell6.c","south",
           "players/kantele/hell2.c","east",
           "players/kantele/hell2.c","west",
         "You are in hell",
         "You are in hell, the smell of sulphur penetrates everything\n",1)
