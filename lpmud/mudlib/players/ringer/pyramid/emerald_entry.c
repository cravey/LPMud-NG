#include "../../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

ONE_EXIT("players/ringer/pyramid/stun4","north",
         "Brilliant green room",
"This room is painted a brilliant shade of green.  The walls seem to glow\n"+
"with a soft green light.  To the south is a narrow hole from which even\n"+
"more green light is coming.  The hole is very small -- it would be\n"+
"difficult to squeeze through.\n",1)


ex_init() {
    add_action("search"); add_verb("search");
    add_action("south"); add_verb("south");
}

south() {
    if (call_other("players/ringer/pyramid/emerald_room","carry_any",
                   this_player())) {
        write("You are carrying too much to squeeze through.\n");
        return 1;
    }
    call_other(this_player(),"move_player",
               "south#players/ringer/pyramid/emerald_room");
    return 1;
}

search(str) {
    if (!str) str = "room";
    if (str != "room")  return 0;
    write("You find nothing unusual.\n");
    say(call_other(this_player(),"query_name") + " searches the room.\n");
    return 1;
}

