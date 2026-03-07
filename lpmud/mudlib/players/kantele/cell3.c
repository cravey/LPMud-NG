#include "std.h"

#undef EXTRA_MOVE1
#define EXTRA_MOVE1\
if (call_other("players/kantele/guard_room","query_gate3")) {\
            write("The celldoor is closed.\n");\
        return 1;\
 }

#undef EXTRA_LONG
#define EXTRA_LONG\
if (call_other("players/kantele/guard_room","query_gate2")) {\
        write("A heavy steel celldoor leads north, the door is closed.\n");\
}\
if (!call_other("players/kantele/guard_room","query_gate2")) {\
            write("A heavy steel celldoor leads north, the door is open.\n");\
}\
    if (str == "celldoor" || str == "door") {\
if (call_other("players/kantele/guard_room","query_gate2")) {\
            write("The celldoor is closed.\n");\
            return 1;\
}\
write("The celldoor is open.\n");\
            return 1;\
}

ONE_EXIT("players/kantele/dungeon_corridor.c","east",
         "A small dirty cell",
         "In a small dirty cell.\n",0)
