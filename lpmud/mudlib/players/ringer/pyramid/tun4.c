#include "../../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

#undef EXTRA_RESET
#define EXTRA_RESET\
       ex_reset();

#undef EXTRA_LONG
#define EXTRA_LONG\
       ex_long();

int npass,spass;

TWO_EXIT("players/ringer/pyramid/tun3","east",
         "players/ringer/pyramid/mgroom","west",
         "East/west corridor",
"The corridor continues east and west here.  The walls are smooth and\n"+
"uniform and somewhat dusty.\n",0)

ex_reset() {
    npass=0;
    spass=0;
}

ex_long() {
    if (npass) 
      write("There is a passage leading north.\n");
    if (spass) 
      write("There is a passage leading south.\n");
}

ex_init() {
    add_action("search"); add_verb("search");
    add_action("north"); add_verb("north");
    add_action("south"); add_verb("south");
}

north() {
    if (!npass) return 0;
    call_other(this_player(),"move_player",
               "north#players/ringer/pyramid/rubyroom");
    return 1;
}

south() {
    if (!spass) return 0;
    call_other(this_player(),"move_player",
               "north#players/ringer/pyramid/pearlroom");
    return 1;
}

search(str) {
    if (!str) str = "room";
    if (str == "room") {
        call_other("players/ringer/pyramid/tun3","osrch","east");
        say(call_other(this_player(),"query_name") + " searches the room.\n");
        if (call_other("players/ringer/pyramid/tun1","query_stones")==0 &&
            !npass) {
            write(
"You notice that a stone is set back in the wall to the north like a door\n"+
"slightly ajar.  You find a passage leading north.\n");
            npass=1;
        }
        if (call_other("players/ringer/pyramid/tun1","query_stones")==3 &&
            !spass) {
            write(
"You notice that a stone is set back in the wall to the south like a door\n"+
"slightly ajar.  You find a passage leading south.\n");
            spass=1;
        }
        return 1;
    }
}
