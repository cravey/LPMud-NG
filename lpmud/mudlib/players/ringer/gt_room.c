#include "../../room/std.h"

#undef EXTRA_RESET
#define EXTRA_RESET\
        ex_reset();

ONE_EXIT("players/ringer/gt2", "west",
	   "You are in the Gorn's treasury.",
"The Gorn keeps all his valuables here. You cannot seem to find the way you\n"
+"came in here in the first place.\n",1)

object money, ring, scroll, item;

ex_reset() {
    int i;
    if (!present("money")) {
        money = clone_object("obj/money");
        call_other(money, "set_money", 200+random(300));
        move_object(money, this_object());
    }
    if (!present("copper ring")) {
        ring = clone_object("players/ringer/armourm");
        call_other(ring,"set_name","copper ring");
        call_other(ring,"set_real_id","ring of protection");
        call_other(ring,"set_short","a copper ring");
        call_other(ring,"set_value",125);
        call_other(ring,"set_weight",1);
        call_other(ring,"set_ac",1);
        call_other(ring,"set_alias","ring");
        call_other(ring,"set_type","protection ring");
        call_other(ring,"set_long","a finely crafted copper ring.\n");
        move_object(ring,this_object());
    }
    if (!present("ugly scroll")) {
	scroll = clone_object("players/ringer/shelt");
        call_other(scroll,"set_id","ugly scroll");
        call_other(scroll,"set_alias","scroll");
        call_other(scroll,"set_value",200);
        call_other(scroll,"set_weight",1);
        call_other(scroll,"set_short","an ugly scroll");
        call_other(scroll,"set_read","The print is smeared.\n");
        call_other(scroll,"set_action","shelter player/ringer/mg_sh");
	move_object(scroll, this_object());
    }
    if (!present("turquoise ring")) {
        item = clone_object("players/ringer/ring1");
        call_other(item,"set_name","turquoise ring");
        call_other(item,"set_alias","ring");
        call_other(item,"set_short","A turquoise ring");
        move_object(item,this_object());
    }        
}

store(ob) {
    move_object(ob,this_object());
}



