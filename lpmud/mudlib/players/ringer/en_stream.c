#include "../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
            ex_init();

ONE_EXIT("players/ringer/en_valley", "north",
	   "You are near a clear running stream",
"There is a clear running stream here.  Its waters sparkle in the sunshine.\n",
1)

ex_init() {
    add_action("south"); add_verb("south");
    add_action("enter"); add_verb("enter");
}

south() {
    return enter("stream");
}

enter(str) {
    if (str=="stream")  {
        call_other(this_player(),"move_player","south#players/ringer/stream");
        return 1;
    }
}

