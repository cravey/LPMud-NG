#include "std.h"


object sphinx;
#undef EXTRA_RESET
#define EXTRA_RESET\
     if (!sphinx  || !present(sphinx)) {\
         sphinx  = clone_object("obj/treasure");\
         call_other(sphinx, "set_id", "sphinx");\
         call_other(sphinx, "set_short", "A huge stone Sphinx stands here");\
         call_other(sphinx, "set_weight", 1000);\
         move_object(sphinx, this_object());\
         }

ONE_EXIT("players/kantele/west_bridge.c","north",
         "The Sphinx room",
         "This is the Sphinx room.\n", 1)
