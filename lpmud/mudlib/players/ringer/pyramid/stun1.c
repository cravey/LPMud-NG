#include "../../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

TWO_EXIT("players/ringer/pyramid/pit1","south",
         "players/ringer/pyramid/stun2","west",
         "Secret tunnel",
"This seems to be an auxiliary tunnel used by the pyramid makers.  It\n"+
"continues to the west and a small hole leads south.\n",0)

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

