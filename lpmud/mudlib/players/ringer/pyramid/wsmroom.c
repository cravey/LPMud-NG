#include "../../../room/std.h"

object gd1,item;

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

#undef EXTRA_RESET
#define EXTRA_RESET\
        my_reset();


ONE_EXIT("players/ringer/pyramid/wcor", "east",
	   "A priest's dressing room",
"This is in a small room. It appears to have been used for preparations\n"+
"of those involved in various pagan rites.\n",0)

ex_init() {
    add_action("search"); add_verb("search");
}

search(str) {
    if (!str) str = "room";
    if (str != "room") return 0;
    write("You find nothing unusual.\n");
    return 1;
}

my_reset() {
    int i;
    if (!present("black snake")) {
        i = 0;
        while(i<2) {
            gd1 = clone_object("obj/monster");
            call_other(gd1, "set_name", "black snake");
            call_other(gd1, "set_alias", "snake");
            call_other(gd1, "set_level", 3);
            call_other(gd1, "set_wc",8);
            call_other(gd1, "set_ac",3);
            call_other(gd1, "set_al", -60);
            call_other(gd1, "set_short", "A black snake");
            call_other(gd1, "set_aggressive", 1);
            move_object(gd1,this_object());
            i = i + 1;
        }
    }
}    


