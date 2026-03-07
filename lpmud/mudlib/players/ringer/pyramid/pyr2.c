#include "../../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

TWO_EXIT("players/ringer/pyramid/pyr1","south",
         "players/ringer/pyramid/pyr3","north",
         "Long hallway",
"This is a long stone hallway.  The floor and walls seem made of large\n"+
"slabs of sandstone.  The workmanship is awesome.  The seams between stones\n"+
"are straight and smooth.\n",0)

ex_init() {
    add_action("search"); add_verb("search");
}

search(str) {
    if (!str) str = "room";
    if (str == "room") {
        write("You find nothing unusual.\n");
        say(call_other(this_player(),"query_name") + " searches the room.\n");
        return 1;
    }
}

