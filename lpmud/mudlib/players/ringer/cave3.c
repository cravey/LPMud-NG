#include "../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

ONE_EXIT("players/ringer/cave2", "south",
	   "You are in a glowing cave",
"You are in a glowing cave.  There is a strange doorway leading north.  The\n"
+"doorway shimmers with a blood red glow. Above this doorway is a sign.  Its\n"
+"words seem to be made of fire.\n",1)

ex_init() {
    add_action("north"); add_verb("north");
    add_action("read"); add_verb("read");
}

read(str) {
    if (str == "sign") {
        write("The sign says:  Gateway of the dead.\n");
        return 1;
    }
}

north() {
    if (!call_other(this_player(),"query_ghost")) {
        write("A strong magical force repells you!\n");
        return 1;
    }
    call_other(this_player(),"move_player","north#players/ringer/bsr2");
    call_other(this_player(),"remove_ghost");
    return 1;
}

