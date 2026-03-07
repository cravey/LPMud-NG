#include "../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
        my_init();


ONE_EXIT("players/ringer/highpt", "south",
	   "You are overlooking a cliff",
"This is the edge of a very high, very sheer cliff.  The cliff is so high\n"
+"you cannot see the base.  The view from here is absolutely spectacular.\n"
+"Only a suicidal maniac would jump over the edge.\n",1)

my_init() {
    add_action("jump"); add_verb("jump");
}

jump(str) {
    object ob;
    if (str == "off" || str =="off cliff" || str == "over edge") {
        ob = clone_object("players/ringer/fall");
        say(call_other(this_player(),"query_name")+" jumps off the cliff.\n");
        move_object(this_player(),ob);
        call_other(ob,"fall",4500);
        return 1;
    }
}
