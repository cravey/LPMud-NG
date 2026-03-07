#include "std.h"

object jewel;
object rockmole;

extra_reset() {
    if (!jewel || environment(jewel) != this_object()) {
	jewel = clone_object("obj/treasure");
        call_other(jewel, "set_id", "ruby");
        call_other(jewel, "set_short", "A small ruby");
        call_other(jewel, "set_long", "The ruby looks almost flawless as it glistens in the light.\n");
        call_other(jewel, "set_weight", 1);
        call_other(jewel, "set_value", 100);
	move_object(jewel, this_object());
    }
    if (!rockmole || !living(rockmole)) {
	object amulet;
	rockmole = clone_object("obj/monster");
	call_other(rockmole, "set_name", "rockmole");
	call_other(rockmole, "set_short", "An ugly disgusting rockmole");
        call_other(rockmole, "set_alias", "mole");
	call_other(rockmole, "set_level", 9);
	call_other(rockmole, "set_wc", 14);
        call_other(rockmole, "set_eps", 1000000);
        call_other(rockmole, "set_ac", 8);
        call_other(rockmole, "set_al", -200);
        call_other(rockmole, "set_aggressive", 1);
	move_object(rockmole, this_object());
	amulet = clone_object("obj/armor");
	call_other(amulet, "set_name", "amulet");
	call_other(amulet, "set_short", "A platinum amulet");
        call_other(amulet, "set_long", "Staring into the diamond jewel on the amulet\n" +
                   "you see a blinding star of light.\n");
        call_other(amulet, "set_type", "amulet");
        call_other(amulet, "set_ac", -2);
	call_other(amulet, "set_value", 1500);
        call_other(amulet, "set_weight", 2);
        call_other(amulet, "set_arm_light", 4);
	move_object(amulet, rockmole);
    }
}

#undef EXTRA_RESET
#define EXTRA_RESET\
    extra_reset();

realm() { return "NT"; }

ONE_EXIT("players/crimson/underworld3", "west",
         "Rockmole Lair",
         "It looks like something is keeping a nest here.  You are in a\n" +
         "small cavernous opening.  To the west you see the ledge you landed\n" +
         "on.  Bones are scattered about the room. \n", 0)
