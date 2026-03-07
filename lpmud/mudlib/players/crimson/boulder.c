#include "std.h"
int moved;

#undef EXTRA_RESET
#define EXTRA_RESET\
        moved=1;
#undef EXTRA_INIT
#define EXTRA_INIT\
    add_action("push"); add_verb("push");\
    add_action("north"); add_verb("north");

north() {
    if (moved == 2) {
        write("You enter the small hole.\n");
call_other(this_player(), "move_player", "north#players/kantele/cellar_passage");
    return 1;
    } else 
    if (moved == 1) {
    write("You try with all of your might to walk through the boulder but fail.\n");
    return 1;
   }
}

push(arg) {
if (arg != "barrel" && arg != "boulder" || moved == 2) {
write("push what ?\n");
        return 1;
} else {
    if (call_other(this_player(), "query_level", 0) <200) {
        write("You are not strong enough to move the boulder.\n");
        say(call_other(this_player(), "query_name", 0) +
            " tries to move the boulder, but fails.\n");
        return 1;
    }
    write("You move the boulder to reveal a passage leading north.\n");
    moved=2;
        say(call_other(this_player(), "query_name", 0) +
            " moves the boulder.\n");
        return 1;
    }
}

id(str) {
    if (str == "stone")
        return 1;
    if (str == "boulder")
        return 1;
}

#undef EXTRA_LONG
#define EXTRA_LONG\
    if (moved == 1)\
write("A large boulder is against the north wall.\n");\
    if (moved == 2)\
write("A large boulder is next to a hole in the north cavern wall.\n");


ONE_EXIT("players/crimson/underworld8","west",
         "Passage",
         "The temperature is better here but, is that\n" +
         "laughter you hear in the distance? And where\n" +
         "on earth could it be coming from?\n", 0)
