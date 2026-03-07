#include "../../room/std.h"

#undef EXTRA_RESET
#define EXTRA_RESET\
        ex_reset();

ONE_EXIT("players/ringer/ref1", "south",
	   "You are in a hidden room.",
"This room was hidden by a secret doorway.  It seems Gurfnel's servants used\n"
+"to keep valuables here.  They must have also had some large beast kept \n"
+"to watch it, since the room is littered with petrified droppings.\n",0)

object money, item;

ex_reset() {
    int i;
    if (!present("money")) {
        money = clone_object("obj/money");
        call_other(money, "set_money", 100+random(100));
        move_object(money, this_object());
    }
    if (!present("golden staff")) {
        item = clone_object("players/ringer/spitem");
        call_other(item,"set_id","golden staff");
        call_other(item,"set_real_id","staff of Gurfnel");
        call_other(item,"set_charges",5);
        call_other(item,"set_spell",4);
        call_other(item,"set_alias","staff");
        call_other(item,"set_short","A golden staff");
        call_other(item,"set_value",100);
        call_other(item,"set_weight",1);
        move_object(item,this_object());
    }
        
    if (!present("grey wand")) {
        item = clone_object("players/ringer/spitem");
        call_other(item,"set_id","grey wand");
        call_other(item,"set_real_id","wand of guardian reduction");
        call_other(item,"set_charges",5);
        call_other(item,"set_spell",6);
        call_other(item,"set_blast_id","evil guardian");
        call_other(item,"set_blast_dam",60);
        call_other(item,"set_alias","wand");
        call_other(item,"set_short","A grey wand");
        call_other(item,"set_value",100);
        call_other(item,"set_weight",1);
        move_object(item,this_object());
    }
}




