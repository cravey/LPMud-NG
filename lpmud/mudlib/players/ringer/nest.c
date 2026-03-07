#include "../../room/std.h"

#undef EXTRA_RESET
#define EXTRA_RESET\
        ex_reset();

ONE_EXIT("players/ringer/dn_valley", "east",
	   "You are in the raven's nest",
"You are in a tall tree where the raven has a nest.\n",1)

object money, ring;

ex_reset() {
    int i;
    i = random(300) + 100;
    if (!money) {
        money = clone_object("obj/money");
        call_other(money, "set_money", i);
        move_object(money, this_object());
    }
    if (!present("silver ring")) {
      ring = clone_object("players/ringer/mitem");
      call_other(ring,"set_id","silver ring");
      call_other(ring,"set_alias","ring");
      call_other(ring,"set_wear","The ring is too small");
      call_other(ring,"set_short","A small silver ring");
      call_other(ring,"set_value",50);
      call_other(ring,"set_weight",1);
      call_other(ring,"set_real_id","silver ring");
      move_object(ring,this_object());
  }
}

