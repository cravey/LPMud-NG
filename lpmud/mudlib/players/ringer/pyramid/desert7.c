#include "../../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

FOUR_EXIT("players/ringer/pyramid/desert3","south",
          "players/ringer/pyramid/desert11","north",
          "players/ringer/pyramid/desert2","east",
          "players/ringer/pyramid/desert9","west",
          "A vast desolate desert",
"A desert stretches far into the distance to the north, south, east and \n"+
"west.\n",1)

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





