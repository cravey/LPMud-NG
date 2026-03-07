#include "std.h"

object fizzywig;

extra_reset() {
    if (!fizzywig || !living(fizzywig)) {
	object scroll;
        object crystal;
	fizzywig = clone_object("obj/monster");
	call_other(fizzywig, "set_name", "fizzywig");
	call_other(fizzywig, "set_short", "Fizzywig the Icelord");
        call_other(fizzywig, "set_long", "Fizzywig looks like a tired grey old man.\n");
        call_other(fizzywig, "set_alias", "icelord");
	call_other(fizzywig, "set_level", 19);
	call_other(fizzywig, "set_wc", 20);
        call_other(fizzywig, "set_eps", 1000000);
        call_other(fizzywig, "set_ac", 8);
        call_other(fizzywig, "set_al", 1000);
        call_other(fizzywig, "set_race", "human");
        call_other(fizzywig, "set_chance", 25);
        call_other(fizzywig, "set_spell_mess1", "Fizzywig invokes a cone of cold\n" +
           " from a scroll.\n");
        call_other(fizzywig, "set_spell_mess2", "Fizzywig read a scroll and hits\n" +
           " you with a cone of cold!\n");
        call_other(fizzywig, "set_spell_dam", 40);
        call_other(fizzywig, "set_aggressive", 0);
        move_object(fizzywig, this_object());
        scroll = clone_object("players/crimson/.scroll");
	move_object(scroll, fizzywig);
        crystal = clone_object("players/crimson/.crystal");
        move_object(crystal, fizzywig);
    }
}

realm() { return "NT"; }
#undef EXTRA_RESET
#define EXTRA_RESET\
    extra_reset();

ONE_EXIT("players/crimson/iceunder4", "west",
         "Fizzywig's hidey hole",
         "How strange! Someone must be hiding out here. There are linens folded\n"+
         "neatly over a floormat.  Light seems to eminate from nowhere.  Various\n" +
         "personals are scattered about the squarely carved cavern room.\n", 1)
