#include "std.h"
object wagon;

#undef EXTRA_RESET
#define EXTRA_RESET\
    extra_reset();

extra_reset() {
    if (!wagon || !environment(wagon)) {
    wagon = clone_object("players/crimson/wagonentry");
    move_object(wagon, this_object());
    }
}

#undef EXTRA_INIT
#define EXTRA_INIT\
    add_action("enter"); add_verb("enter");\
    add_action("enter"); add_verb("board");\
    add_action("enter"); add_verb("climb");

enter(str) {
    if (str == "wagon" || str == "into wagon") {
    move_object(this_player(), "players/crimson/wag");
    say(call_other(this_player(), "query_name")+" boards the wagon.\n");
    write("You board the wagon.\n");
    call_other(this_player(), "look", 0);
    return 1;
    } else
    if (!str) {
    return 0;
    } else
    if (str) {
    return 0;
    }
}

ONE_EXIT("room/city/a2", "north",
          "Bazaar Wagon",
          "Here the ground slopes downward into a green open valley.\n"+
          "There aren't as many people here.  The bustle of the city\n"+
          "bazaar can be heard off in the distance to the north.\n"+
          "Strange and mystical music heightens your senses.\n"+
          "Could this be? YES!, its a gypsy encampment.\n", 1)
