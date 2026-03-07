#include "../../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

FOUR_EXIT("players/ringer/pyramid/ncor1","south",
         "players/ringer/pyramid/nwtun","northwest",
         "players/ringer/pyramid/ntun","north",
         "players/ringer/pyramid/netun","northeast",
         "Strange juction",
"This is a very strange junction.  Instead of a perpendicular hallway\n"+
"crossing, corridors lead north, south, northeast and northwest.  The\n"+
"air is very musty here.\n",0)

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

