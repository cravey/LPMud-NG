#include "../../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

#undef EXTRA_RESET
#define EXTRA_RESET\
        my_reset();

ONE_EXIT("players/ringer/pyramid/nhall4","south",
         "Crypt of Xyphelod",
"Before you lies the crypt of xyphelod. It is large and ornate -- the\n"+
"walls are decorated with picturesque scenes of a fertile river valley.\n"+
"The crypt appears to have been sacked many times throughout the ages,\n"+
"although the caretakers seem to try to keep the place up.\n",1)

ex_init() {
    add_action("search"); add_verb("search");
}

object item, guard;
int treasure;

search(str) {
    if (!str) str = "room";
    if (str != "room")  return 0;
    if (present("golem")) {
        write("The golem prevents you.\n");
        return 1;
    }
    if (random(100) < 50 || !treasure) {
        write("You find nothing unusual.\n");
    } else {
        write("You discover some hidden treasure.\n");
        treasure=0;
        item = clone_object("obj/treasure");
        call_other(item,"set_id","staff of Xyphelod");
        call_other(item,"set_alias","staff");
        call_other(item,"set_short","The staff of Xyphelod");
        call_other(item,"set_long",
"The staff is engraved with the name 'Xyphelod'.\n");
        call_other(item,"set_value",150);
        call_other(item,"set_weight",1);
        move_object(item,this_object());
        item = clone_object("obj/money");
        call_other(item,"set_money",random(200) + 50);
        move_object(item,this_object());
    }
    say(call_other(this_player(),"query_name") + " searches the room.\n");
    return 1;
}



my_reset() {
    treasure=1;
    if (!present("stone golem")) {
        guard = clone_object("obj/monster");
        call_other(guard,"set_name","stone golem");
        call_other(guard,"set_alias","golem");
        call_other(guard,"set_level",5);
        call_other(guard,"set_ac",5);
        call_other(guard,"set_wc",12);
        call_other(guard,"set_long","The golem is made of stone.\n");
        call_other(guard,"set_aggressive",1);
        call_other(guard,"set_al",-175);
        move_object(guard,this_object());
    }
}    
