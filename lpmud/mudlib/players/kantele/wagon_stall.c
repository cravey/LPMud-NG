#include "std.h"

object ferrari;
#undef EXTRA_RESET
#define EXTRA_RESET\
     if (!ferrari  || !present(ferrari)) {\
         ferrari = clone_object("obj/treasure");\
         call_other(ferrari, "set_id", "ferrari");\
         call_other(ferrari, "set_alias", "red ferrari");\
         call_other(ferrari, "set_short", "A brand new Ferrari stands here");\
         call_other(ferrari, "set_weight", 1000);\
         call_other(ferrari, "set_value", 1000);\
         move_object(ferrari, this_object());\
         }

TWO_EXIT("players/kantele/courtyard" , "east",
         "players/kantele/mechanic_room" , "up",
         "The wagon stall",
         "This is the place where visitors park their wagons.\n" +
         "A stairway leads up, and an open portal leads out to the\n" +
         "courtyard. ",1)

