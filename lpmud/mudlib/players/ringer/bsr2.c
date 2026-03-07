#include "../../room/std.h"

#undef EXTRA_RESET
#define EXTRA_RESET\
        ex_reset();

ONE_EXIT("players/ringer/cave3", "south",
	   "You are in the bleak room",
"You are in a black, bleak room filled with darkness.\n",1)

object blade;

ex_reset() {
    int i;
    if (!present("black bleak blade of blood")) {
        blade = clone_object("obj/weapon");
        call_other(blade,"set_name","black bleak blade of blood");
        call_other(blade,"set_alias","blade");
        call_other(blade,"set_class",12);
        call_other(blade,"set_short","The black bleak blade of blood");
        call_other(blade,"set_long",
"The sword is black.  It has some runes engraved in red.\n");
        call_other(blade,"set_value",2000);
        call_other(blade,"set_weight",1);
        call_other(blade,"set_hit_func",this_object());
        call_other(blade,"set_wield_func",this_object()); 
        move_object(blade,this_object());
    }
}

wield() {
    object sword;
    int al,wc;
    sword = present("black bleak blade of blood",this_player());
    al = call_other(this_player(),"query_alignment");
    if (al > 0) {
        write("The sword melts in your hands!\n");
        call_other(this_player(),"hit_player",random(40));
        destruct(sword);
        return 0;
    }
    wc = -al/20;
    if (wc > 18) wc = 18;
    call_other(sword,"set_class",wc);
    return 1;
}

weapon_hit(a) {
    int al;
    if (call_other(a,"id","ringer_pt_dragon")) {
        write("The sword sizzles at it strikes.\n");
        call_other(a,"set_ac",0);
        return 100;
    }
    if (call_other(a,"id","ringer_pt_gold_dragon")) {
        write("The sword sizzles at it strikes.\n");
        call_other(a,"set_ac",0);
        return 100;
    }
    al = call_other(a,"query_alignment")/10;
    if (al > 0) al = al/10;
    if (al > 10) al = 10;
    return al;
}
