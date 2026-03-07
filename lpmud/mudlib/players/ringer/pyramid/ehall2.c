#include "../../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

TWO_EXIT("players/ringer/pyramid/ehall3","east",
         "players/ringer/pyramid/ehall1","west",
         "East/west hallway",
"The hallway continues east and west here.  The walls are smooth and\n"+
"uniform and somewhat dusty.\n",0)

ex_init() {
    add_action("search"); add_verb("search");
}

search(str) {
    if (!str) str = "room";
    if (str == "room") {
        write(
   "You notice something slightly different about the walls to the east.\n"+
   "They seem to be pitted with dozens of small holes on both sides of \n"+
   "the corridor.\n");
        say(call_other(this_player(),"query_name") + " searches the room.\n");
        return 1;
    }
}
