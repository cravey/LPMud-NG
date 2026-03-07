#include "../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

TWO_EXIT("players/ringer/dp_wood2","south",
         "players/ringer/pyramid/desert1","north",
         "Desert edge",
"The forest ends abruptly here with tall trees giving way to smaller\n"+
"shrub brush. The change is foliage is clearly due to lack of moisture.\n"+
"Indeed further to the north looms a great desert, stretching out of sight.\n",
1)

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

