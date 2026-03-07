#include "../../room/std.h"

#undef EXTRA_RESET
#define EXTRA_RESET\
        ex_reset();

ONE_EXIT("players/ringer/courtyd", "south",
	   "You are in the brilliant sword room.",
"You are in a room filled with brilliant light.\n",1)

object blade;

ex_reset() {
    int i;
    if (!present("shining sword")) {
        blade = clone_object("obj/weapon");
        call_other(blade,"set_name","shining sword");
        call_other(blade,"set_alias","sword");
        call_other(blade,"set_class",12);
        call_other(blade,"set_short","A brilliant shining sword");
        call_other(blade,"set_long",
"The sword is very finely crafted and polished.\n");
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
    sword = present("shining sword",this_player());
    al = call_other(this_player(),"query_alignment");
    if (al < 0) {
        write("The sword melts in your hands!\n");
        call_other(this_player(),"hit_player",random(40));
        destruct(sword);
        return 0;
    }
    wc = al/20;
    if (wc > 18) wc = 18;
    call_other(sword,"set_class",wc);
    return 1;
}

weapon_hit(a) {
    int al;
    if (call_other(a,"id","ringer_cr_dragon")) {
        write("The sword sizzles at it strikes.\n");
        call_other(a,"set_ac",0);
        return 100;
    }
    if (call_other(a,"id","ringer_cr_demon")) {
      write("The sword sizzles at it strikes.\n");
        call_other(a,"set_ac",0);
        return 100;
    }
    al = -call_other(a,"query_alignment")/10;
    if (al > 0) al = al/10;
    if (al > 10) al = 10;
    return al;
}
