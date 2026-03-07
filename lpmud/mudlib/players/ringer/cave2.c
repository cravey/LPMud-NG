#include "../../room/std.h"

object gd1,item;

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

#undef EXTRA_RESET
#define EXTRA_RESET\
        my_reset();


ONE_EXIT("players/ringer/cave1", "south",
	   "You are in a glowing room",
"You are in a strange cavern.  The room is shrouded in mist and the mist\n"
+"seems to glow with a soft red light. To the north another room glows \n"
+"in much the same way.\n",1)

ex_init() {
    add_action("north"); add_verb("north");
}

north() {
   if (present("guardian")) {
     write("The evil guardian blocks your way!\n");
     return 1;
   }
   call_other(this_player(),"move_player","north#players/ringer/cave3");
   return 1;
}

my_reset() {
    int i;
    if (!present("evil guardian")) {
        i = 0;
        while(i<2) {
            gd1 = clone_object("obj/monster");
            call_other(gd1, "set_name", "evil guardian");
            call_other(gd1, "set_alias", "guardian");
            call_other(gd1, "set_level", 16);
            call_other(gd1, "set_al", -60);
            call_other(gd1, "set_ac",6);
            call_other(gd1, "set_wc",10);
            call_other(gd1, "set_hp",500);
            call_other(gd1, "set_short", "An evil guardian");
            call_other(gd1, "set_long",
"The guardian stands 9 ft tall. His eyes glow red and brimstone puffs from\n"
+"his nostrils.\n");
            call_other(gd1, "set_aggressive", 1);
            call_other(gd1, "set_chance",30);
            call_other(gd1, "set_spell_mess1","The guardian breathes fire.");
            call_other(gd1, "set_spell_mess2","The guardian breathes fire.");
            call_other(gd1, "set_spell_dam",40);
            move_object(gd1,this_object());
            i = i + 1;
        }
    }
}    
