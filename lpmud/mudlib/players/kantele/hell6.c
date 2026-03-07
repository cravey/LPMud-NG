#include "std.h"

object barbed_devil;
#undef EXTRA_RESET
#define EXTRA_RESET\
new_reset();

new_reset() {
    if (!barbed_devil || !living(barbed_devil)) {
        int i;
        i = 2;
        while(i>0) {
            i -= 1;
            barbed_devil = clone_object("obj/monster");
            call_other(barbed_devil, "set_name", "devil");
            call_other(barbed_devil, "set_alias", "barbed devil");
            call_other(barbed_devil, "set_level",14);
            call_other(barbed_devil, "set_short", "A barbed devil is here");
            call_other(barbed_devil, "set_wc",14);
            call_other(barbed_devil, "set_ac",5);
            call_other(barbed_devil, "set_aggressive", 1);
            call_other(barbed_devil, "set_move_at_reset");
            call_other(barbed_devil, "set_whimpy");
           call_other(barbed_devil, "set_chance",30);
           call_other(barbed_devil, "set_spell_mess1",
                "The barbed devil lashes out with its tail.");
           call_other(barbed_devil, "set_spell_mess2",
                "You are hit by the barbed tail");
           call_other(barbed_devil, "set_spell_dam",30);
            move_object(barbed_devil, this_object());
           }
}
}
FOUR_EXIT("players/kantele/hell2.c","north",
           "players/kantele/hell7.c","south",
           "players/kantele/hell5.c","east",
           "players/kantele/hell6.c","west",
         "You are in hell",
         "You are in hell, the smell of sulphur penetrates everything\n",1)
