#include "std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
    add_action("look"); add_verb("look");\
    add_action("read"); add_verb("read");

look(str) {

    if (str == "down") {
    move_object(this_player(), "players/crimson/greeting");
    call_other(this_player(), "look", 0);
    move_object(this_player(), "players/crimson/eastrail");
    return 1;
    } else
    if (str == "at sign") {
    write("The nursery is open.  Visitors are welcome.\n");
    return 1;
    }
}

read(str) {
 
    if (str == "sign") {
    write("The nursery is open. Visitors are welcome.\n");
    return 1;
    }
}

TWO_EXIT("players/crimson/winge", "north",
         "players/crimson/were", "east",
          "East Rail",
          "You are on the east side of a circular railing. Above is the\n" +
          "dome and below you (if you look down) is a spectacular view.\n" +
          "To the east is an open door with a sign on it.\n", 1)
