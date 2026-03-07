#include "../../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

#undef EXTRA_RESET
#define EXTRA_RESET\
        my_reset();

ONE_EXIT("players/ringer/pyramid/whall3","east",
         "Crypt of the court magician",
"Before you lies an old crypt.  The walls are decorated with pictures of\n"+
"magicians conjuring monsters and casting spells.  You decide this must \n"+
"be the crypt of the court magician.  The crypt appears to have been \n"+
"sacked many times over the ages and has been stripped of much of its \n"+
"former glory.\n",0)

ex_init() {
    add_action("search"); add_verb("search");
}

object item, guard;
int treasure;

search(str) {
    if (!str) str = "room";
    if (str != "room")  return 0;
    if (present("spirit")) {
        write("The night spirit prevents you.\n");
        return 1;
    }
    if (random(100) < 50 || !treasure) {
        write("You find nothing unusual.\n");
    } else {
        write("You discover some hidden treasure.\n");
        treasure=0;
        item = clone_object("obj/treasure");
        call_other(item,"set_id","silver chain");
        call_other(item,"set_alias","chain");
        call_other(item,"set_short","A silver chain");
        call_other(item,"set_long",
"The chain is small and looks like it once held some magical value.\n");
        call_other(item,"set_value",100);
        call_other(item,"set_weight",1);
        move_object(item,this_object());
        item = clone_object("obj/money");
        call_other(item,"set_money",random(100) + 10);
        move_object(item,this_object());
    }
    say(call_other(this_player(),"query_name") + " searches the room.\n");
    return 1;
}



my_reset() {
    treasure=1;
    if (!present("night spirit")) {
        guard = clone_object("obj/monster");
        call_other(guard,"set_name","night spirit");
        call_other(guard,"set_alias","spirit");
        call_other(guard,"set_level",5);
        call_other(guard,"set_ac",5);
        call_other(guard,"set_wc",12);
        call_other(guard,"set_long","The spirit is made of darkness.\n");
        call_other(guard,"set_aggressive",1);
        call_other(guard,"set_al",-175);
        move_object(guard,this_object());
    }
}    
