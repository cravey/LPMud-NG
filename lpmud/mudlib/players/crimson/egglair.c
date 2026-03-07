#include "std.h"

object egga;
object eggb;
object eggc;
object eggd;
object egge;
object giant1;
object giant2;
object giant3;
object robe;
object player;
int i;

extra_reset() {
    if (!egga || environment(egga) != this_object()) {
        egga = clone_object("players/crimson/.egg");
	move_object(egga, this_object());
    }
    if (!eggb || environment(eggb) != this_object()) {
        eggb = clone_object("players/crimson/.egg");
        move_object(eggb, this_object());
    }
    if (!eggc || environment(eggc) != this_object()) {
        eggc = clone_object("players/crimson/.egg");
        move_object(eggc, this_object());
    }
    if (!eggd || environment(eggd) != this_object()) {
        eggd = clone_object("players/crimson/.egg");
        move_object(eggd, this_object());
    }
    if (!egge || environment(egge) != this_object()) {
        egge = clone_object("players/crimson/.egg");
        move_object(egge, this_object());
    }
    if (!giant2 || !living(giant2)) {
        player = this_player();
	giant2 = clone_object("obj/monster");
	call_other(giant2, "set_name", "giant");
	call_other(giant2, "set_alias", "stone giant");
	call_other(giant2, "set_level", 15);
	call_other(giant2, "set_short", "A stone giant");
	call_other(giant2, "set_wc", 20);
	call_other(giant2, "set_ac", 2);
	call_other(giant2, "set_al", -700);
        call_other(giant2, "set_a_chat_chance", 10);
        call_other(giant2, "load_a_chat", "A giant says: I KNEW I heard somethin out there!\n");
        call_other(giant2, "load_a_chat", "A giant says: Get em Grok!\n");
        call_other(giant2, "set_aggressive", 1);
	move_object(giant2, this_object());
        robe = clone_object("obj/armor");
	call_other(robe, "set_name", "robe");
	call_other(robe, "set_alt_name", "green robe");
	call_other(robe, "set_short", "A large green robe");
        call_other(robe, "set_ac", 1);
        call_other(robe, "set_type", "cloak");
	call_other(robe, "set_weight", 2);
	call_other(robe, "set_value", 2000);
	move_object(robe, giant2);
        call_other(robe, "wear", "robe");
    }
    if (!giant3 || !living(giant3)) {
	giant3 = clone_object("obj/monster");
	call_other(giant3, "set_name", "giant");
	call_other(giant3, "set_alias", "stone giant");
	call_other(giant3, "set_level", 15);
	call_other(giant3, "set_short", "A stone giant");
	call_other(giant3, "set_wc", 20);
	call_other(giant3, "set_ac", 2);
	call_other(giant3, "set_al", -700);
	call_other(giant3, "set_aggressive", 1);
	move_object(giant3, this_object());
        robe = clone_object("obj/armor");
	call_other(robe, "set_name", "robe");
	call_other(robe, "set_alt_name", "green robe");
	call_other(robe, "set_short", "A large green robe");
        call_other(robe, "set_ac", 1);
        call_other(robe, "set_type", "cloak");
	call_other(robe, "set_weight", 2);
	call_other(robe, "set_value", 2000);
	move_object(robe, giant3);
        call_other(robe, "wear", "robe");
    }
    if (!giant1 || !living(giant1)) {
	giant1 = clone_object("obj/monster");
	call_other(giant1, "set_name", "giant");
	call_other(giant1, "set_alias", "stone giant");
	call_other(giant1, "set_level", 15);
	call_other(giant1, "set_short", "A stone giant");
	call_other(giant1, "set_wc", 20);
	call_other(giant1, "set_ac", 2);
	call_other(giant1, "set_al", -700);
	call_other(giant1, "set_aggressive", 1);
	move_object(giant1, this_object());
        robe = clone_object("obj/armor");
	call_other(robe, "set_name", "robe");
	call_other(robe, "set_alt_name", "green robe");
	call_other(robe, "set_short", "A large green robe");
        call_other(robe, "set_ac", 1);
        call_other(robe, "set_type", "cloak");
	call_other(robe, "set_weight", 2);
	call_other(robe, "set_value", 2000);
	move_object(robe, giant1);
        call_other(robe, "wear", "robe");
    }
}

#undef EXTRA_RESET
#define EXTRA_RESET\
    extra_reset();

ONE_EXIT("room/giant_lair", "east",
	 "Giants conference of human bashing",
	 "You are at the yearly conference of human bashing,\n" +
	 "organized by the giants.\n", 1)
