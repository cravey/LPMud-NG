#include "std.h"

object stonegolem;

extra_reset() {
    if (!stonegolem || !living(stonegolem)) {
	object pearl;
	stonegolem = clone_object("obj/monster");
	call_other(stonegolem, "set_name", "stonegolem");
	call_other(stonegolem, "set_short", "A stonegolem");
        call_other(stonegolem, "set_alias", "golem");
	call_other(stonegolem, "set_level", 9);
	call_other(stonegolem, "set_wc", 14);
        call_other(stonegolem, "set_ac", 8);
        call_other(stonegolem, "set_al", 100);
        call_other(stonegolem, "set_eps", 5500000);
        call_other(stonegolem, "set_aggressive", 1);
	move_object(stonegolem, this_object());
	pearl = clone_object("obj/treasure");
	call_other(pearl, "set_id", "pearl");
	call_other(pearl, "set_short", "A black pearl");
        call_other(pearl, "set_long", "The pearl looks like a tiny black heart \n" +
            "and is unusually cold to the touch.\n");
	call_other(pearl, "set_value", 1500);
        call_other(pearl, "set_weight", 1);
	move_object(pearl, stonegolem);
    }
}

realm() { return "NT"; }

#undef EXTRA_RESET
#define EXTRA_RESET\
    extra_reset();

ONE_EXIT("players/crimson/underworld4", "east",
         "Stonegolem",
         "A small pool bubbles in the center of the room here.  You can see\n" +
         "a stream leading out of it flowing west. The walls have an odd\n" +
         "silver huge to them. \n", 0)
