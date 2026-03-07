#include "../../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

#undef EXTRA_RESET
#define EXTRA_RESET\
        my_reset();

ONE_EXIT("players/ringer/pyramid/nwtun","south",
         "A brilliant blue room",
"This room is painted completely blue.  Light fills the room from an\n"+
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

object gem;

my_reset() {
    if (!present("jasper")) {
        gem = clone_object("players/ringer/mitem");
        call_other(gem,"set_id","large jasper");
        call_other(gem,"set_real_id","large jasper");
        call_other(gem,"set_alias","gem");
        call_other(gem,"set_short","A large jasper gem");
        call_other(gem,"set_value",400);
        call_other(gem,"set_weight",2);
        move_object(gem,this_object());
    }
}    
