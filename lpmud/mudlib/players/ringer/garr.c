#include "../../room/std.h"

object gd1,item;

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

#undef EXTRA_RESET
#define EXTRA_RESET\
        my_reset();


ONE_EXIT("players/ringer/groo3", "west",
	   "You are in the groo hall",
"You are in a large room.  This is where all the groo meet for comunal\n"
+"meals (usually serving unfortunate adventurers...)  To the east a\n"
+"passage continues",0)

ex_init() {
    add_action("east"); add_verb("east");
}

east() {
   if (present("groo")) {
     write("A groo blocks your way!\n");
     return 1;
   }
   call_other(this_player(),"move_player","east#players/ringer/gprin");
   return 1;
}

my_reset() {
    int i;
    if (!present("groo")) {
        i = 0;
        while(i<7) {
            gd1 = clone_object("obj/monster");
            call_other(gd1, "set_name", "groo");
            call_other(gd1, "set_level", 2);
            call_other(gd1, "set_al", -60);
            call_other(gd1, "set_short", "A groo");
            call_other(gd1, "set_long",
                       "The groo is short and ugly.  He looks like he wants to eat you.\n");
            call_other(gd1, "set_aggressive", 1);
            move_object(gd1,this_object());
            if (random(10) > 5) {
                item = clone_object("obj/weapon");
                call_other(item,"set_name","short spear");
                call_other(item,"set_alias","spear");
                call_other(item,"set_class",7);
                call_other(item,"set_short","A short spear");
                call_other(item,"set_value",175);
                call_other(item,"set_weight",2);
                move_object(item,gd1);
                call_other(gd1,"init_command","wield spear");
            }
            if (random(10) > 5) {
                item = clone_object("obj/armor");
                call_other(item,"set_name","leather armor");
                call_other(item,"set_alias","leather");
                call_other(item,"set_ac",3);
                call_other(item,"set_short","A leather armor");
                call_other(item,"set_value",110);
                call_other(item,"set_weight",2);
                move_object(item,gd1);
                call_other(gd1,"init_command","wear leather");
            }
            i = i + 1;
        }
    }
}    
