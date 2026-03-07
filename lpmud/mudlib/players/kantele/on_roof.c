#include "std.h"

#undef EXTRA_INIT
#define EXTRA_INIT add_action("down"); add_verb("jump");

TWO_EXIT("players/kantele/stable","north",
         "players/kantele/upper_hall","south",
         "You are on the roof",
         "You are on the roof, if you hadn't known better you could swear\n" +
         "The building to the north looks like a stable. You can think you\n" +
         "can smell sulphur here. ",1)
down(arg) {
    if (arg != "down" && arg != "out")  {
       write("You throw yourself over the edge !\n");
       write("You hit the dirt hard......\n");
    call_other(this_player(), "hit_player",20);
say(call_other(this_player(), "query_name") + " throws himself down from the rof.\n");
call_other(this_player(), "move_player", "down#players/kantele/garden_west");
return 1;
    }
}
