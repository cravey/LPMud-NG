#include "../../room/std.h"

#undef EXTRA_RESET
#define EXTRA_RESET\
        ex_reset();

ONE_EXIT("players/ringer/en_for9", "east",
	   "You are in Mulf's home", 
"You are in Mulf's home.  Mulf seems to have created this place to live in.\n"
+"It seems to be in the middle of the forest, but a magical spell keeps out\n"
+"the elements.\n",1)

object money,item;

ex_reset() {
    int i;
    if (!present("money")) {
        money = clone_object("obj/money");
        call_other(money, "set_money", 100+random(100));
        move_object(money, this_object());
    }
    i=0;
    if (!present("trapstone")) {
        while(i<4) {
            item = clone_object("players/ringer/trapstone");
            call_other(item,"set_id","red stone");
            call_other(item,"set_alias","stone");
            call_other(item,"set_short","A translucent red stone");
            call_other(item,"set_long",
"The stone is beautiful. Deep inside light plays in its facets resembling\n"
+"little tongues of flame.\n");
            call_other(item,"set_value",100);
            call_other(item,"set_weight",1);
            move_object(item,this_object());       
            i=i+1;
        }
    }
    if (!present("fam_item")) {
        call_other("players/ringer/rd_lair","ex_reset");
    }
}



