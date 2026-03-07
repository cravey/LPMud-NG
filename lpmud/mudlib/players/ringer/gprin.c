#include "../../room/std.h"

object gd1,item;

#undef EXTRA_INIT
#define EXTRA_INIT my_init();

#undef EXTRA_RESET
#define EXTRA_RESET\
        my_reset();


ONE_EXIT("players/ringer/garr", "west",
	   "You are in the groo throne room",
"You in a royal groo throne room.  It is indicative of what passes for\n"
+"royal decor for groos -- dirty and primitive.\n",0)

int pass;

my_init() {
    add_action("east"); add_verb("east");
    add_action("search"); add_verb("search");
}

search(str) {
    if(!str) str="room";
    if (str=="room") {
        write("You find a passage leading east!\n");
        say(call_other(this_player(),"query_name") + " finds a passage "
            +"leading east.\n");
        pass=1;
        return 1;
    }
    return 0;
}

east() {
    if (pass) {
        if (present("groo prince")) {
            write("The groo prince bars the way!\n");
            return 1;
        }
        call_other(this_player(),"move_player","east#players/ringer/gptr");
        return 1;
    }
}


my_reset() {
    int i;
    if (!present("groo prince")) {
	gd1 = clone_object("obj/monster");
	call_other(gd1, "set_name", "groo prince");
        call_other(gd1, "set_alias","prince");
	call_other(gd1, "set_level", 7);
	call_other(gd1, "set_al", -100);
	call_other(gd1, "set_short", "The prince of the groo");
	call_other(gd1, "set_long",
       "The prince is slightly taller than most groos.  Definitely uglier.\n"
                   +"There is a small black orb floating next to him.\n");
	call_other(gd1, "set_aggressive", 1);
        call_other(gd1, "set_chance",20);
        call_other(gd1, "set_spell_mess1", 
     "On the prince's command the orb emits a burst of magic missiles!\n");
        call_other(gd1, "set_spell_mess2", 
     "On the prince's command the orb emits a burst of magic missiles!\n");
        call_other(gd1,"set_spell_dam",20);
        move_object(gd1,this_object());
        item = clone_object("players/ringer/spitem");
        call_other(item,"set_id","black orb");
        call_other(item,"set_real_id","orb of missiles");
        call_other(item,"set_charges",10);
        call_other(item,"set_spell",1);
        call_other(item,"set_alias","orb");
        call_other(item,"set_short","A floating black orb");
        call_other(item,"set_value",50);
        call_other(item,"set_weight",0);
        move_object(item,gd1);
        item = clone_object("obj/weapon");
        call_other(item,"set_name","spear");
        call_other(item,"set_alias","spear");
        call_other(item,"set_class",8);
        call_other(item,"set_short","A spear");
        call_other(item,"set_value",300);
        call_other(item,"set_weight",4);
        move_object(item,gd1);
        call_other(gd1,"init_command","wield spear");
        item = clone_object("obj/armor");
        call_other(item,"set_name","studded leather armor");
        call_other(item,"set_alias","leather");
        call_other(item,"set_ac",4);
        call_other(item,"set_short","A studded leather armor");
        call_other(item,"set_value",300);
        call_other(item,"set_weight",4);
        move_object(item,gd1);
        call_other(gd1,"init_command","wear leather");
    }
} 

