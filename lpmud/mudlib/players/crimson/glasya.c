#include "std.h"

object glasya;

extra_reset() {
    if (!glasya || !living(glasya)) {
	object scroll;
        object crystal;
	glasya = clone_object("obj/monster");
	call_other(glasya, "set_name", "glasya");
        call_other(glasya, "set_short", "Glasya the Consort of Mammon (Princess of Hell)");
        call_other(glasya, "set_long", "Glasya has beautiful human features but also has huge clawed wings and horns.\n");
        call_other(glasya, "set_alias", "devil");
	call_other(glasya, "set_level", 19);
	call_other(glasya, "set_wc", 25);
        call_other(glasya, "set_eps", 1000000);
        call_other(glasya, "set_ac", 12);
        call_other(glasya, "set_al", -1000);
        call_other(glasya, "set_race", "demon");
        call_other(glasya, "set_chance", 50);
        call_other(glasya, "set_spell_mess1", "Glasya arches her back and spews forth a bolt of lighting!\n");
        call_other(glasya, "set_spell_mess2", "Glasya spits a lighting bolt on you!\n");
        call_other(glasya, "set_spell_dam", 40);
        call_other(glasya, "set_aggressive", 1);
        move_object(glasya, this_object());
        scroll = clone_object("players/crimson/.lighting");
	move_object(scroll, glasya);
        crystal = clone_object("players/crimson/.lighting");
        move_object(crystal, glasya);
    }
}

realm() { return "NT"; }

#undef EXTRA_RESET
#define EXTRA_RESET\
    extra_reset();

ONE_EXIT("players/crimson/hordlings", "south",
         "Glasya's Pit",
         "Oh YUCK! What IS that SMELL?! Litter is scattered all over the\n" +
         "the room. As you gaze around you infrequently see letters of lust\n"+
         "from Glasya to Mammon.\n", 0)
