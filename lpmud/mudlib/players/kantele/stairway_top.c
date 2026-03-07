#include "std.h"


reset(arg) {
    if (!arg)
	set_light(1);
}

long(str) {
    write("You are on a landing in the stairway, this place gives you \n"+
          "a bad feeling, you feel truley depresed at the tought of\n"+
          "going further up the stairway.\n");
}

short() {
    return "You are on the upper landing";
}

init() {
    add_action("up"); add_verb("up");
    add_action("down"); add_verb("down");
}


down()  {
   call_other(this_player(), "move_player", "down#players/kantele/stairway_up");
}

up()  {
   write("As you start to walk up the stair, A pit opens under your feet\n");
   write("You fall, down into the pit.........\n\n\n\n");
   call_other(this_player(), "move_player", "up#players/kantele/snake_pit");
}
