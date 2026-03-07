#include "../../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

#undef EXTRA_RESET
#define EXTRA_RESET\
        my_reset();

#undef EXTRA_LONG
#define EXTRA_LONG ex_long();

int chute;

ex_long() {
    if (chute) {
        write("A dark chute leads south.\n");
    }
}

ONE_EXIT("players/ringer/pyramid/ehall3","west",
         "Crypt of Xyphelod's family",
"Before you lies an old crypt.  The walls are decorated with pictures of\n"+
"the royal family as they were in the prime of their lives.  Each one in\n"+
"turn was buried here.  The crypt appears to have been sacked many times\n"+
"over the ages and has been stripped of much of its former glory.\n",0)

ex_init() {
    add_action("search"); add_verb("search");
    add_action("south"); add_verb("south");
}

south() {
    if (!chute) return 0;
    if (chute) {
        write(
"You slide down the chute.  A large slab opens up and you are deposited\n"+
"in a stone hallway.  The slab quietly slides back into place with a dull\n"+
"thud.\n");
        call_other(this_player(),"move_player",
                   "south#players/ringer/pyramid/pyr2");
        return 1;
    }
}


object guard;

search(str) {
    if (!str) str = "room";
    if (str != "room")  return 0;
    if (present("spirit")) {
        write("The fire spirit prevents you.\n");
        return 1;
    }
    if (random(100)>50) {
        write("You find nothing unusual.\n");
    } else {
        write(
"You find a passage to the south. It appears to be a chute.  If you go\n"+
"through it you may not be able to climb back up.\n");
        chute=1;
    }
    say(call_other(this_player(),"query_name") + " searches the room.\n");
    return 1;
}



my_reset() {
    chute=0;
    if (!present("fire spirit")) {
        guard = clone_object("obj/monster");
        call_other(guard,"set_name","fire spirit");
        call_other(guard,"set_alias","spirit");
        call_other(guard,"set_level",5);
        call_other(guard,"set_ac",5);
        call_other(guard,"set_wc",15);
        call_other(guard,"set_long","The spirit is made of fire.\n");
        call_other(guard,"set_aggressive",1);
        call_other(guard,"set_al",-175);
        move_object(guard,this_object());
    }
}    
