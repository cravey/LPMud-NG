#include "../../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

#undef EXTRA_LONG
#define EXTRA_LONG ex_long();

ex_long() {
    if (call_other("players/ringer/pyramid/nhall3","query_pit")) {
        write("There is an open pit to the south.\n");
    }
}

TWO_EXIT("players/ringer/pyramid/nhall3","south",
         "players/ringer/pyramid/xyfalse","north",
         "End of north/south hallway",
"The hallway ends here.  To the north is a larger room.\n",0)

ex_init() {
    add_action("search"); add_verb("search");
}

search(str) {
    if (!str) str = "room";
    if (str == "room") {
        if (!call_other("players/ringer/pyramid/nhall3","query_pit")){
            write(
"You notice something slightly different about the floor to the south.\n"+
"Upon closer examination you see that the stone slab for that section\n"+
"must be a trap door.\n");
        } else 
          write("You notice an open pit to the south.\n");
        say(call_other(this_player(),"query_name") + " searches the room.\n");
        return 1;
    }
}
