#include "../../room/std.h"

#undef EXTRA_RESET
#define EXTRA_RESET\
        ex_reset();

ONE_EXIT("players/ringer/trap3", "south",
	   "You are in the dwarf's treasury.",
"The dwarf keeps all his valuables here.\n",0)

object dwarf,money, item;

ex_reset() {
    int i;
    if (!present("dwarf")) {
        dwarf = clone_object("obj/monster");
        call_other(dwarf,"set_name","short dwarf");
        call_other(dwarf,"set_alias","dwarf");
        call_other(dwarf,"set_short","short dwarf");
        call_other(dwarf,"set_long","The dwarf is short and wimpy.\n");
        call_other(dwarf,"set_level",4);
        call_other(dwarf,"set_ac",2);
        call_other(dwarf,"set_wc",2);
        call_other(dwarf,"set_aggressive",1);
        call_other(dwarf,"set_al",-150);
        move_object(dwarf,this_object());
    }
    if (!present("money")) {
        money = clone_object("obj/money");
        call_other(money, "set_money", 200+random(400));
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
    if (!present("bronze helm")) {
        item = clone_object("obj/armor");
        call_other(item,"set_name","bronze helm");
        call_other(item,"set_alias","helm");
        call_other(item,"set_ac",1);
        call_other(item,"set_type","helmet");
        call_other(item,"set_short","A bronze helm");
        call_other(item,"set_value",50);
        call_other(item,"set_weight",3);
        move_object(item,this_object());
    }
}




