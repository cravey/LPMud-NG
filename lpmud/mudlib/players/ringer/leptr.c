#include "../../room/std.h"

#undef EXTRA_RESET
#define EXTRA_RESET\
        ex_reset();

ONE_EXIT("players/ringer/en_for8", "west",
	   "You are at the end of the rainbow",
"You are at the end of the rainbow.\n",1)

object money;

ex_reset() {
    int i;
    if (!present("money")) {
        money = clone_object("obj/money");
        call_other(money, "set_money", 200+random(300));
        move_object(money, this_object());
    }
}



