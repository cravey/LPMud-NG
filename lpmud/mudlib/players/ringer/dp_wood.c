#include "../../room/std.h"

object chipmunk;

#undef EXTRA_INIT
#define EXTRA_INIT my_init();

#undef EXTRA_RESET
#define EXTRA_RESET\
        my_reset();


THREE_EXIT("players/ringer/long_hill", "south",
         "players/ringer/dk_trail","west",
         "players/ringer/for_cliff","east",
	   "You are in a deep wood",
"The trees here are very dense and dark.  You feel very uncomfortable here.\n"
+"To the west the wood gets even darker.\n" ,1)

int i;
object item;

my_init() {
   add_action("search"); add_verb("search");
}

my_reset() {
    i = random(4)+1;
    if (!chipmunk || !living(chipmunk)) {
	chipmunk = clone_object("obj/monster");
	call_other(chipmunk, "set_name", "rabid chipmunk");
        call_other(chipmunk, "set_alias", "chipmunk");
	call_other(chipmunk, "set_level", 3);
	call_other(chipmunk, "set_al", -6);
	call_other(chipmunk, "set_short", "A rabid chipmunk");
	call_other(chipmunk, "set_long",
		   "The chipmunk is foaming at the mouth.\n");
	call_other(chipmunk, "set_aggressive", 1);
        move_object(chipmunk,this_object());
    }
}    

search(str) {
    if (!str) str = "room";
    if (str == "room") {
        if (random(10) < 5 || i == 0 ) {
            write("You find nothing unusual.\n");
            return 1;
        }
        if (i==1) {
            write("You find something under some leaves...a yellow wand!\n");
            item = clone_object("players/ringer/spitem");
            call_other(item,"set_id","yellow wand");
            call_other(item,"set_real_id","wand of identify");
            call_other(item,"set_charges",2+random(2));
            call_other(item,"set_spell",2);
            call_other(item,"set_alias","wand");
            call_other(item,"set_short","A yellow wand");
            call_other(item,"set_value",20);
            call_other(item,"set_weight",2);
            move_object(item,this_object());
        }
        if (i==2) {
            write("You find something under some leaves...a green stone!\n");
            item = clone_object("players/ringer/spitem");
            call_other(item,"set_id","green stone");
            call_other(item,"set_real_id","stone of healing");
            call_other(item,"set_charges",2);
            call_other(item,"set_spell",5);
            call_other(item,"set_alias","stone");
            call_other(item,"set_short","A green stone");
            call_other(item,"set_value",75);
            call_other(item,"set_object","players/ringer/pdtr");
            call_other(item,"set_weight",2);
            move_object(item,this_object());
        }
        if (i==3) {
            write("You find something under some leaves...a dagger!\n");
            item = clone_object("obj/weapon");
            call_other(item,"set_name","dagger");
            call_other(item,"set_class",6);
            call_other(item,"set_alias","knife");
            call_other(item,"set_short","A dagger");
            call_other(item,"set_value",25);
            call_other(item,"set_weight",2);
            move_object(item,this_object());
        }
        if (i==4) {
            write("You find something under some leaves...an orange wand!\n");
            item = clone_object("players/ringer/spitem");
            call_other(item,"set_id","orange wand");
            call_other(item,"set_real_id","wand of identify");
            call_other(item,"set_charges",5+random(3));
            call_other(item,"set_spell",2);
            call_other(item,"set_alias","wand");
            call_other(item,"set_short","A orange wand");
            call_other(item,"set_value",25);
            call_other(item,"set_weight",2);
            move_object(item,this_object());
        }
        i=0;
       return 1;
    }
}
