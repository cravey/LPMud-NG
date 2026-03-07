#include "../../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

TWO_EXIT("players/ringer/pyramid/desert6","south",
          "players/ringer/pyramid/desert5","west",
          "A vast desolate desert",
"A desert stretches far into the distance to the south and west.\n",1)

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





