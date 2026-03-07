#include "../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
        ex_init();

TWO_EXIT("players/ringer/stream", "north",
          "players/ringer/old_shack", "south",
	   "You are in the southern end of a lovely valley",
"You are down in the southern end of the valley.  A stream is just to the.\n"
+"north.  To the west there is a large tree.  To the south an old ugly\n"
+"shack looks rather out of place. Its door is open.\n",1)

ex_init() {
    add_action("west"); add_verb("west");
}

west() {
    if (call_other(this_player(),"query_name") == "black raven") {
        call_other(this_player(),"move_player","west#players/ringer/nest");
    } else {
        tell_object(this_player(),"The tree's branches are too high for you to reach.\n");
    }
    return 1;
}
