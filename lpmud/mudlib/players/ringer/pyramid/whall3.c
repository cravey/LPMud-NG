#include "../../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

TWO_EXIT("players/ringer/pyramid/whall2","east",
         "players/ringer/pyramid/mgfalse","west",
         "East/west hallway",
"The hallway ends to the west in a large room.  The walls are smooth and\n"+
"uniform and somewhat dusty.\n",0)

ex_init() {
    add_action("search"); add_verb("search");
    if (call_other(this_player(),"query_ghost")) return;
    if (call_other("players/ringer/pyramid/whall1","query_stone")==0) {
        write(
"\nSuddenly a volley of needle-like darts fly through the air from holes in\n"+
"walls.\n");
        write("You are hit many times.\n");
        call_other(this_player(),"hit_player",10+random(12));
    }
}

search(str) {
    if (!str) str = "room";
    if (str == "room") {
        write(
"You notice something slightly different about the walls here.  There seem\n"+
"to be dozens of holes in the walls on either side of the passage.\n");
        say(call_other(this_player(),"query_name") + " searches the room.\n");
        return 1;
    }
}
