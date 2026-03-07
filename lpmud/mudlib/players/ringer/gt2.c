#include "../../room/std.h"

#undef EXTRA_RESET
#define EXTRA_RESET\
        ex_reset();

ONE_EXIT("players/ringer/gt_room", "east",
	   "You are in the Gorn's lounge.",
"The Gorn seems to have used this place to relax.\n",1)

object scroll;

ex_reset() {
    int i;
    if (!scroll || !present(scroll)) {
        scroll = clone_object("obj/treasure");
        call_other(scroll,"set_id","dusty scroll");
        call_other(scroll,"set_alias","scroll");
        call_other(scroll,"set_short","An old dusty scroll");
        call_other(scroll,"set_long",
"The scroll is old and dusty.  You read it carefully and see that the Gorns\n"
+"are masters of teleportation, having tamed blink dogs and wolves.  It also\n"
+"says that Gorns use rings to enable them to call one another to their aid.\n"
);
        call_other(scroll,"set_read",
"You read it carefully and see that the Gorns are masters of teleportation,\n"
+"having tamed blink dogs and wolves.  It also says that Gorns use rings to\n"
+"enable them to call one another to their aid.\n");
        call_other(scroll,"set_value",10);
        call_other(scroll,"set_weight",1);
        move_object(scroll,this_object());
    }
}

