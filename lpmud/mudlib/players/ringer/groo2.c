#include "../../room/std.h"

object gd1,item;

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

#undef EXTRA_RESET
#define EXTRA_RESET\
        my_reset();


ONE_EXIT("players/ringer/groo1", "south",
	   "You are in at a groo guard post",
"You are in a small room, little larger than a wide spot in the tunnel.\n"
+"It seems to be a guard post of some sort. A passage leads east.\n",0)

ex_init() {
    add_action("east"); add_verb("east");
}

east() {
   if (present("groo guard")) {
     write("The guard blocks your way!\n");
     return 1;
   }
   call_other(this_player(),"move_player","east#players/ringer/groo3");
   return 1;
}

my_reset() {
    int i;
    if (!gd1 || !living(gd1)) { 
    i = 0;
    while(i<2) {
	gd1 = clone_object("obj/monster");
	call_other(gd1, "set_name", "groo guard");
	call_other(gd1, "set_alias", "groo");
	call_other(gd1, "set_level", 3);
	call_other(gd1, "set_al", -60);
	call_other(gd1, "set_short", "A groo guard");
	call_other(gd1, "set_long",
"The guard is short and ugly.  He looks like he wants to eat you.\n");
	call_other(gd1, "set_aggressive", 1);
        move_object(gd1,this_object());
        item = clone_object("obj/weapon");
        call_other(item,"set_name","short spear");
        call_other(item,"set_alias","spear");
        call_other(item,"set_class",7);
        call_other(item,"set_short","A short spear");
        call_other(item,"set_value",175);
        call_other(item,"set_weight",2);
        move_object(item,gd1);
        call_other(gd1,"init_command","wield spear");
        item = clone_object("obj/armor");
        call_other(item,"set_name","leather armor");
        call_other(item,"set_alias","leather");
        call_other(item,"set_ac",3);
        call_other(item,"set_short","A leather armor");
        call_other(item,"set_value",110);
        call_other(item,"set_weight",2);
        move_object(item,gd1);
        call_other(gd1,"init_command","wear leather");
        i = i + 1;
      }
      item = clone_object("obj/treasure");
      call_other(item,"set_id","iron key");
      call_other(item,"set_alias","key");
      call_other(item,"set_short","iron key");
      call_other(item,"set_weight",1);
      call_other(item,"set_value",20);
      move_object(item,gd1);
    }
}    
