#include "std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
    add_action("look"); add_verb("look");

look(str) {
string down;

    if (str == "down") {
    move_object(this_player(), "players/crimson/greeting");
    call_other(this_player(), "look", 0);
    move_object(this_player(), "players/crimson/westrail");
    return 1;
    }
}

TWO_EXIT("players/crimson/wingw", "north",
         "players/crimson/fetch", "west",
          "West Rail",
          "You are on the west side of a circular railing. Above is the\n" +
          "dome and below you (if you look down) is a spectacular view.\n" +
          "To the west is an open doorway.\n", 1)
