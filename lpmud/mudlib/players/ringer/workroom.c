#include "../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
         ex_init();

ONE_EXIT("players/ringer/mg_sh_store","east",
         "Ringer's workroom",
	 "This is an office strewn with paper and misc alchemy equipment.\n"
	+"It's clear ringer has several new magic items in the works. Its\n"
	+"hard to make heads or tails out of any of it.\n",1)


ex_init() {
    add_action("fire"); add_verb("fire");
    add_action("miss"); add_verb("miss");
    add_action("light"); add_verb("light");
    add_action("iden"); add_verb("id");
    add_action("tor"); add_verb("torch");
}

miss() {
    object item;
    if (call_other(this_player(),"query_name") != "Ringer") return 0;
    item = clone_object("players/ringer/spitem");
    call_other(item,"set_id","yellow crystal");
    call_other(item,"set_real_id","crystal of missiles");
    call_other(item,"set_charges",5);
    call_other(item,"set_spell",1);
    call_other(item,"set_alias","crystal");
    call_other(item,"set_short","A yellow crystal");
    call_other(item,"set_value",50);
    call_other(item,"set_weight",0);
    move_object(item,this_player());
    return 1;
}

iden() {
    object item;
    if (call_other(this_player(),"query_name") != "Ringer") return 0;
    item = clone_object("players/ringer/spitem");
    call_other(item,"set_id","blue orb");
    call_other(item,"set_real_id","orb of identify");
    call_other(item,"set_charges",5);
    call_other(item,"set_spell",2);
    call_other(item,"set_alias","orb");
    call_other(item,"set_short","A blue orb");
    call_other(item,"set_value",50);
    call_other(item,"set_weight",0);
    move_object(item,this_player());
    return 1;
}

light() {
    object item;
    if (call_other(this_player(),"query_name") != "Ringer") return 0;
    item = clone_object("players/ringer/spitem");
    call_other(item,"set_id","white wand");
    call_other(item,"set_real_id","wand of lightning");
    call_other(item,"set_charges",5);
    call_other(item,"set_spell",3);
    call_other(item,"set_alias","wand");
    call_other(item,"set_short","A white wand");
    call_other(item,"set_value",75);
    call_other(item,"set_weight",0);
    move_object(item,this_player());
    return 1;
}

fire() {
    object item;
    if (call_other(this_player(),"query_name") != "Ringer") return 0;
    item = clone_object("players/ringer/spitem");
    call_other(item,"set_id","black staff");
    call_other(item,"set_real_id","staff of fire");
    call_other(item,"set_charges",5);
    call_other(item,"set_spell",4);
    call_other(item,"set_alias","staff");
    call_other(item,"set_short","A long black staff");
    call_other(item,"set_value",100);
    call_other(item,"set_weight",0);
    move_object(item,this_player());
    return 1;
}

tor() {
    object t;
    if (call_other(this_player(),"query_name") != "Ringer") return 0;
    t = clone_object("obj/torch");
    call_other(t, "set_name", "torch");
    call_other(t, "set_fuel", 10000);
    call_other(t, "set_weight", 1);
    move_object(t, this_player());
    return 1;
}
