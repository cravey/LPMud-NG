#include "../../room/std.h"

#undef EXTRA_RESET
#define EXTRA_RESET\
        ex_reset();

ONE_EXIT("players/ringer/pdlair", "south",
	   "You are in dragon's treasure room",
"You are in the platinum dragon's treasure room!\n",1)

object item;
ex_reset() {
    int i;
    if (!present("money")) {
        item = clone_object("obj/money");
        call_other(item,"set_money",2500+random(1000));
        move_object(item,this_object());
    }
    if (!present("stone")) {
        item = clone_object("players/ringer/trapstone");
        call_other(item,"set_id","silvery stone");
        call_other(item,"set_alias","stone");
        call_other(item,"set_short","A silvery stone");
        call_other(item,"set_long","The stone is translucent.  You find yourself wanting to gaze into it.\n");
        call_other(item,"set_value",100);
        call_other(item,"set_weight",1);
        move_object(item,this_object());    
    }   
    if (!present("silver wand")) {
        item = clone_object("players/ringer/spitem");
        call_other(item,"set_id","silver wand");
        call_other(item,"set_real_id","wand of lightning");
        call_other(item,"set_charges",20);
        call_other(item,"set_spell",3);
        call_other(item,"set_alias","wand");
        call_other(item,"set_short","A silver wand");
        call_other(item,"set_value",75);
        call_other(item,"set_weight",1);
        move_object(item,this_object());
    }
    if (!present("black staff")) {
        item = clone_object("players/ringer/spitem");
        call_other(item,"set_id","black staff");
        call_other(item,"set_real_id","staff of fire");
        call_other(item,"set_charges",20);
        call_other(item,"set_spell",4);
        call_other(item,"set_alias","staff");
        call_other(item,"set_short","A black staff");
        call_other(item,"set_value",100);
        call_other(item,"set_weight",1);
        move_object(item,this_object());
    }
    if (!present("green orb")) {
        item = clone_object("players/ringer/spitem");
        call_other(item,"set_id","green orb");
        call_other(item,"set_real_id","orb of healing");
        call_other(item,"set_charges",8);
        call_other(item,"set_spell",5);
        call_other(item,"set_object",this_object());
        call_other(item,"set_alias","orb");
        call_other(item,"set_short","A green glowing orb");
        call_other(item,"set_value",100);
        call_other(item,"set_weight",1);
        move_object(item,this_object());
    }
}

use() {
    write("You feel better.\n");
    call_other(this_player(),"heal_self",random(10)+20);
    return 1;
}
