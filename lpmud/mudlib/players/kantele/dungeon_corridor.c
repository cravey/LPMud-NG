#include "std.h"

#undef EXTRA_INIT
#define EXTRA_INIT add_action("open_door"); add_verb("open");\
add_action("open_door"); add_verb("unlock");

open_door(str) {
if (str == "door" || str == "celldoor") {
   write("It isn't anyway, you can do that.\n");
return 1;
}
}
#undef EXTRA_LONG
#define EXTRA_LONG\
   if (str == "celldoor" || str == "door" || str == "doors") {\
            write("The celldoors is made of steel and looks very tough.\n");\
            return 1;\
}

#undef EXTRA_MOVE3
#define EXTRA_MOVE3\
if (call_other("players/kantele/guard_room","query_gate2")) {\
            write("The celldoor is closed.\n");\
        return 1;\
 }

#undef EXTRA_MOVE4
#define EXTRA_MOVE4\
if (call_other("players/kantele/guard_room","query_gate3")) {\
            write("The celldoor is closed.\n");\
        return 1;\
 }

#undef EXTRA_MOVE2
#define EXTRA_MOVE2\
if (call_other("players/kantele/guard_room","query_gate1")) {\
            write("The celldoor is closed.\n");\
        return 1;\
 }
FOUR_EXIT("players/kantele/guard_room.c","north",
          "players/kantele/cell1.c","south",
          "players/kantele/cell2.c","east",
          "players/kantele/cell3.c","west",
          "A dungeon corridor",
          "A dungeon corridor, you can enter the watch room to the north in the\n" +
          "other directions there are celldoors\n",0)
id(str) {
return str == "celldoor" || str == "door" || str == "doors";}
