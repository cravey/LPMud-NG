#include "std.h"
int moved;

#undef EXTRA_RESET
#define EXTRA_RESET\
        moved=1;
#undef EXTRA_INIT
#define EXTRA_INIT\
    add_action("move"); add_verb("move");\
    add_action("south"); add_verb("south");\

south() {
    if (moved == 2) {
        write("You enter the small hole.\n");
call_other(this_player(), "move_player", "south#players/kantele/cellar_passage");
return 1;
}
else {
return 1;
}
}

move(arg) {
if (arg != "barrel" && arg != "barrels" || moved == 2) {
write("move what ?\n");
        return 1;
} else {
    if (call_other(this_player(), "query_level", 0) <14) {
        write("You are not strong enough to move the barrels.\n");
        say(call_other(this_player(), "query_name", 0) +
            " tries to move the barrels, but fails.\n");
        return 1;
    }
    write("You move the barrels to reveal a secret passage leading south.\n");
    moved=2;
        say(call_other(this_player(), "query_name", 0) +
            " moves some of the barrels.\n");
        return 1;
    }
}

id(str) {
    if (str == "barrel")
        return 1;
    if (str == "barrels")
        return 1;
}

#undef EXTRA_LONG
#define EXTRA_LONG\
    if (moved == 1)\
write("Some large barrels are lying against the south wall.\n");\
    if (moved == 2)\
write("Some large barrels are lying here, you can see a small black hole to the south.\n");\


ONE_EXIT("players/kantele/stairway_down.c","west",
         "You are in a large dark hall",
         "You are in a large dark hall, you can enter the stairway to the west.\n",0)
