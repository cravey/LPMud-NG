#include "../../room/std.h"

object gd1;

#undef EXTRA_LONG
#define EXTRA_LONG\
       ex_long();

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

#undef EXTRA_RESET
#define EXTRA_RESET\
        my_reset();


ONE_EXIT("players/ringer/groo3", "north",
	   "You are outside a groo prison cell",
"You are in a small room.  To the south is a prison cell.\n",0)

int locked;

ex_long() {
    if (!locked) write("The door to the south is open.\n");
}

ex_init() {
    add_action("unlock"); add_verb("unlock");
    add_action("unlock"); add_verb("open");
    add_action("south"); add_verb("south");
}

south() {
   if (!locked) {
       call_other(this_player(),"move_player","south#players/ringer/cell");
       return 1;
   }
   return 0;
}

unlock(str) {
    if (!present("iron key", this_player())) return 0;
    if (str == "cell" || str == "door") {
        if (!locked) {
            write("Its already open!\n"); 
            return 1; 
        }
        write("You unlock and open the prison cell.\n");
        say(call_other(this_player(),"query_name") + " unlocks and opens the prison cell.\n");
        locked = 0;
        return 1;
    }
}

my_reset() {
    locked = 1;
}
