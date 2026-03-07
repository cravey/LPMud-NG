#include "std.h"

object jewel;
object margoyle;

extra_reset() {
    if (!jewel || environment(jewel) != this_object()) {
	jewel = clone_object("obj/treasure");
        call_other(jewel, "set_id", "sapphire");
        call_other(jewel, "set_short", "A small sapphire");
        call_other(jewel, "set_long", "An oval cut sapphire.\n");
        call_other(jewel, "set_weight", 1);
        call_other(jewel, "set_value", 100);
	move_object(jewel, this_object());
    }
    if (!margoyle || !living(margoyle)) {
	object sword;
	margoyle = clone_object("obj/monster");
	call_other(margoyle, "set_name", "margoyle");
	call_other(margoyle, "set_short", "A margoyle");
        call_other(margoyle, "set_long", "A horned stone-faced margoyle.\n");
        call_other(margoyle, "set_alias", "gargoyle");
	call_other(margoyle, "set_level", 19);
        call_other(margoyle, "set_eps", 2000000);
        call_other(margoyle, "set_hp", 400);
	call_other(margoyle, "set_wc", 14);
        call_other(margoyle, "set_chance", 30);
        call_other(margoyle, "set_spell_mess1", "The margoyle nashes his teeth.\n");
        call_other(margoyle, "set_spell_mess2", "The margoyle sinks it's"+
           " stonelike teeth into your flesh.\n");
        call_other(margoyle, "set_spell_dam", 40);
        call_other(margoyle, "set_ac", 10);
        call_other(margoyle, "set_al", -1000);
        call_other(margoyle, "set_aggressive", 1);
	move_object(margoyle, this_object());
	sword = clone_object("obj/weapon");
	call_other(sword, "set_name", "sword");
        call_other(sword, "set_alias", "crom's sword");
	call_other(sword, "set_short", "The sword of Crom");
        call_other(sword, "set_class", 20);
        call_other(sword, "set_long", "A double-edged grey blade with a black" +
         " opal in the hilt.\n");
	call_other(sword, "set_value", 5000);
        call_other(sword, "set_weight", 2);
	move_object(sword, margoyle);
        call_other(sword, "wield", "sword");
    }
}


realm() { return "NT"; }

#undef EXTRA_RESET
#define EXTRA_RESET\
    extra_reset();

ONE_EXIT("players/crimson/stairs2", "west",
         "Margoyle's Hole",
         "Smooth grey walls are all around. The only discernable difference\n" +
         "in the room are the bits of gravel scattered about.\n", 0)
