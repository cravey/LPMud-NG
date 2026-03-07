#include "../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT my_init();

ONE_EXIT("players/ringer/cr2", "east",
	   "You on a ledge of a sheer cliff.",
"You are standing on a ledge of a sheer cliff.  The base of the mountain is\n"
+"straight down about 3300 meters.  Above you, just out of reach, is a tree\n"
+"attemping to growing out of the cliff.  To the east is a dark cave.\n",1)

my_init() {
    add_action("jump"); add_verb("jump");
}

jump(str) {
    object ob;
    if (str == "off" || str =="off cliff" || str == "over edge") {
        ob = clone_object("players/ringer/fall");
        say(call_other(this_player(),"query_name")+" jumps off the cliff.\n");
        move_object(this_player(),ob);
        call_other(ob,"fall",3300);
        return 1;
    }
}
