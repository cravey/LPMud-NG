#include "../../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

TWO_EXIT("players/ringer/pyramid/tun1","east",
         "players/ringer/pyramid/tun3","west",
         "East/west corridor",
"The corridor continues east and west here.  The walls are smooth and\n"+
"uniform and somewhat dusty.\n",0)

ex_init() {
    add_action("search"); add_verb("search");
}

search(str) {
    if (!str) str = "room";
    if (str == "room") {
        call_other("players/ringer/pyramid/tun3","osrch","west");
        say(call_other(this_player(),"query_name") + " searches the room.\n");
        return 1;
    }
}
