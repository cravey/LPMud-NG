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

TWO_EXIT("players/ringer/en_for5", "east",
         "players/ringer/en_for7", "north",
	   "You are deep within a quiet forest",
"You are now deep in the quiet forest. Sunlight streams through the trees.\n"
+"You feel a great sense of enchantment in this place.\n",1)

int passage;

ex_init() {
    add_action("west"); add_verb("west");
    add_action("search"); add_verb("search");
}

search(str) {
    if (!str) str = "room";
    if (str == "room") {
        write("A search of the area reveals a hidden path leading west.\n");
        say(call_other(this_player(),"query_name") + " finds a hidden path leading west.\n");
        passage = 1;
        return 1;
    }
}

ex_long(){
    if (passage) write("There is path leading west.\n");
}

west() {
   if (passage) {
       call_other(this_player(),"move_player","north#players/ringer/en_for9");
       return 1;
   }
   return 0;
}

object gnome;

my_reset() {
    passage=0;
    if (!present("gnome")) {
	gnome = clone_object("obj/monster");
	call_other(gnome, "set_name", "peaceful gnome");
        call_other(gnome, "set_alias", "gnome");
	call_other(gnome, "set_level", 4);
	call_other(gnome, "set_al", 100);
        call_other(gnome, "set_ac",4);
	call_other(gnome, "set_short", "A peaceful gnome");
	call_other(gnome, "set_long",
		   "The gnome is sitting peacefully on a mushroom.\n");
	call_other(gnome, "set_aggressive", 0);
        call_other(gnome, "set_chance",25);
        call_other(gnome, "set_spell_mess1",
                   "The gnome throws magic dust!");
        call_other(gnome, "set_spell_mess2",
                   "The gnome throws magic dust!");
        call_other(gnome, "set_spell_dam",20);
        move_object(gnome,this_object());
    }
}    

