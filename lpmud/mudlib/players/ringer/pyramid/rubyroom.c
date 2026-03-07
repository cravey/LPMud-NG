#include "../../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

#undef EXTRA_RESET
#define EXTRA_RESET\
        my_reset();

ONE_EXIT("players/ringer/pyramid/tun4","south",
         "A crimson red room",
"This room is painted completely red.  Light fills the room from an\n"+
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

object ruby;

my_reset() {
    if (!present("ringer_ruby")) {
        ruby = clone_object("players/ringer/mitem");
        call_other(ruby,"set_id","ruby");
        call_other(ruby,"set_alias","ringer_ruby");
        call_other(ruby,"set_short","A large beautiful ruby");
        call_other(ruby,"set_value",200);
        call_other(ruby,"set_weight",2);
        move_object(ruby,this_object());
    }
}    
