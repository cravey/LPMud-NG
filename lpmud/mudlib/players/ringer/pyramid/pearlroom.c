#include "../../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

#undef EXTRA_RESET
#define EXTRA_RESET\
        my_reset();

ONE_EXIT("players/ringer/pyramid/tun4","north",
         "A whitewashed room",
"This room is painted completely white.  Light fills the room from an\n"+
"unknown source.\n",1)

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

object pearl;

my_reset() {
    if (!present("ringer_pearl")) {
        pearl = clone_object("players/ringer/mitem");
        call_other(pearl,"set_id","pearl");
        call_other(pearl,"set_alias","ringer_pearl");
        call_other(pearl,"set_short","A large beautiful pearl");
        call_other(pearl,"set_value",200);
        call_other(pearl,"set_weight",2);
        move_object(pearl,this_object());
    }
}    
