#include "../../room/std.h"

object gd1,item;
int passage;

#undef EXTRA_LONG
#define EXTRA_LONG\
       ex_long();

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

#undef EXTRA_RESET
#define EXTRA_RESET\
        my_reset();


ONE_EXIT("players/ringer/tun4", "south",
	   "You are in a troll den",
"You are in a large room.  This appears to be a cave troll hibernation spot.\n"
+"(You can tell by the smell, ACK!)\n",0)

ex_long() {
    if (passage) write("There is a passage leading east.\n");
}

ex_init() {
    add_action("search"); add_verb("search");
    add_action("east"); add_verb("east");
}

search(str) {
    if (!str) str = "room";
    if (str != "room") return 0;
    write("You find a passage leading east.\n");
    say(call_other(this_player(),"query_name") + " finds a passage leading east.\n");
    passage = 1;
    return 1;
}

east() {
   if (!passage) return 0;
   if (present("cave troll")) {
     write("A burly troll blocks your way!\n");
     return 1;
   }
   call_other(this_player(),"move_player","east#players/ringer/tun5");
   return 1;
}

my_reset() {
    int i;
    passage = 0;
    if (!present("cave troll")) {
        i = 0;
        while(i<3) {
            gd1 = clone_object("obj/monster");
            call_other(gd1, "set_name", "cave troll");
            call_other(gd1, "set_alias", "troll");
            call_other(gd1, "set_level", 8);
            call_other(gd1, "set_hp",110);
            call_other(gd1, "set_wc",12);
            call_other(gd1, "set_al", -60);
            call_other(gd1, "set_short", "A cave troll");
            call_other(gd1, "set_long",
"The troll is large and ugly.  He looks almost as stupid as he is mean.\n");
            call_other(gd1, "set_aggressive", 1);
            move_object(gd1,this_object());
            i = i + 1;
        }
    }
}    


