#include "../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

#undef EXTRA_RESET
#define EXTRA_RESET\
        my_reset();

#undef EXTRA_LONG
#define EXTRA_LONG\
      ex_long();

ONE_EXIT("players/ringer/en_for7", "south",
	   "You are at the edge of a beatiful chasm",
"You are on the eastern edge of a chasm.  The forest ends here and allows\n"
+"a spectacular view of a mighty river far down below.  A fantastic rainbow\n"
+"arcs gracefully over the chasm.\n",1)

int passage,rainbow;

ex_init() {
    add_action("east"); add_verb("east");
    add_action("north"); add_verb("north");
    add_action("search"); add_verb("search");
}

search(str) {
    if (!str) str = "room";
    if (random(100) < 95) {
        write("You find nothing of interest.\n");
        return 1;
    }
    if (str == "room") {
        write("A search of the area reveals a hidden path leading north.\n");
        say(call_other(this_player(),"query_name") + " finds a hidden path leading north.\n");
        passage = 1;
        return 1;
    }
}

ex_long(){
    if (passage) write("There is path leading north.\n");
}

north() {
   if (passage) {
       call_other(this_player(),"move_player","north#players/ringer/pd1");
       return 1;
   }
   return 0;
}

east() {
   if (rainbow) {
       call_other(this_player(),"move_player","north#players/ringer/leptr");
       return 1;
   }
   return 0;
}

object leprechaun,item;

my_reset() {
    passage = 0;
    rainbow = 0;
    if (!present("leprechaun")) {
	leprechaun = clone_object("obj/monster");
	call_other(leprechaun, "set_name", "peaceful leprechaun");
        call_other(leprechaun, "set_alias", "leprechaun");
	call_other(leprechaun, "set_level", 4);
	call_other(leprechaun, "set_al", 100);
        call_other(leprechaun, "set_ac",4);
	call_other(leprechaun, "set_short", "A peaceful leprechaun");
	call_other(leprechaun, "set_long",
		   "The leprechaun is picking clover and potatoes.\n");
	call_other(leprechaun, "set_aggressive", 0);
        call_other(leprechaun, "set_chance",25);
        call_other(leprechaun, "set_spell_mess1",
                   "The leprechaun throws a fit!");
        call_other(leprechaun, "set_spell_mess2",
                   "The leprechaun throws a fit!");
        call_other(leprechaun, "set_spell_dam",20);
        move_object(leprechaun,this_object());
        if (!present("knurled stick")) {
            item = clone_object("players/ringer/spitem");
            call_other(item,"set_id","walking stick");
            call_other(item,"set_real_id","staff of leprechauns");
            call_other(item,"set_charges",1);
            call_other(item,"set_spell",5);
            call_other(item,"set_object",this_object());
            call_other(item,"set_alias2","staff");
            call_other(item,"set_alias","stick");
            call_other(item,"set_short","A walking stick");
            call_other(item,"set_value",50);
            call_other(item,"set_weight",1);
            move_object(item,leprechaun);
        }
    }
}    

rain_bow() {
    write("The rainbow seems to take on solid form!\n");
    say("The rainbow seems to take on solid form!\n");
    rainbow=1;
}

use() {
    write("The stick shakes and hums mystically.");
    call_other(environment(this_player()),"rain_bow");
    return 1;
}




