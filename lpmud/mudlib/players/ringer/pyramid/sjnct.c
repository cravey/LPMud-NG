#include "../../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

FOUR_EXIT("players/ringer/pyramid/spass1","south",
         "players/ringer/pyramid/ncor1","north",
         "players/ringer/pyramid/ecor1","east",
         "players/ringer/pyramid/tun1","west",
         "Stone junction",
"Here two hallways cross.  There are corridors leading north, south, east\n"+
"and west.\n",0)

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

