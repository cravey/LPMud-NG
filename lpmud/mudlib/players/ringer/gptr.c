#include "../../room/std.h"

#undef EXTRA_RESET
#define EXTRA_RESET\
        ex_reset();

ONE_EXIT("players/ringer/gprin", "west",
	   "You are in the groo prince's treasury.",
"The groo prince keeps all his valuables here.\n",0)

object money, item;

ex_reset() {
    int i;
    if (!present("money")) {
        money = clone_object("obj/money");
        call_other(money, "set_money", 300+random(400));
        move_object(money, this_object());
    }
    if (!present("blue wand")) {
        item = clone_object("players/ringer/spitem");
        call_other(item,"set_id","blue wand");
        call_other(item,"set_real_id","wand of lightning");
        call_other(item,"set_charges",3+random(4));
        call_other(item,"set_spell",3);
        call_other(item,"set_alias","wand");
        call_other(item,"set_short","A blue wand");
        call_other(item,"set_value",75);
        call_other(item,"set_weight",0);
        move_object(item,this_object());
    }
    if (!present("dark cloak")) {
        item = clone_object("obj/armor");
        call_other(item,"set_name","dark cloak");
        call_other(item,"set_alias","cloak");
        call_other(item,"set_ac",1);
        call_other(item,"set_type","cloak");
        call_other(item,"set_short","A dark cloak");
        call_other(item,"set_value",100);
        call_other(item,"set_weight",2);
        move_object(item,this_object());
    }
}




