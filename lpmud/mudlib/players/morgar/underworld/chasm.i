# 1 "players/morgar/underworld/chasm.c"

# 1 "/usr/users/jseidman/lpmud/mudlib/room/std.h"







































































































































































# 2 "players/morgar/underworld/chasm.c"

object balrog;




extra_init() {
    add_action("spellfail"); add_verb("fireball");
    add_action("spellfail"); add_verb("shock");
    add_action("spellfail"); add_verb("missile");
}




reset(arg) { extra_reset(); if (arg) return; set_light( 0); }short() {    if (set_light(0))	return 
	 "The Balrog's chasm";    return "dark room";}init() {    add_action("move"); add_verb( "up");    extra_init();}move() {        call_other(this_player(), "move_player",  "up" + "#" +"players/morgar/underworld/brink");    return 1;}long(str) {    if (set_light(0) == 0){       write("It is dark.\n");       return;    }        write(
	 "You are in a deep chasm, the lair of the dreaded balrog.\n");    write("    The only obvious exit is " +  "up" + ".\n");}

realm() { return "under"; }

extra_reset() {
    object treasure;
    if (!balrog || !living(balrog)) {
	balrog = clone_object("obj/monster");
	call_other(balrog, "set_name", "balrog");
	call_other(balrog, "set_alias", "demon");
	call_other(balrog, "set_level", 19);
        call_other(balrog, "set_hp", 400);
        call_other(balrog, "set_ep", 1000000);
	call_other(balrog, "set_short", "A balrog");
	call_other(balrog, "set_wc", 30);
	call_other(balrog, "set_ac", 7);
	call_other(balrog, "set_al", -900);
	call_other(balrog, "set_aggressive", 1);
        call_other(balrog, "set_chance",35);
        call_other(balrog, "set_spell_mess1",
                   "The balrog casts a deadly spell.");
        call_other(balrog, "set_spell_mess2",
                   "The balrog wounds you with a deadly spell.");
        call_other(balrog, "set_spell_dam",35);
	move_object(balrog, this_object());
	treasure = clone_object("obj/treasure");
	call_other(treasure, "set_id", "emerald");
	call_other(treasure, "set_alias", "gem");
	call_other(treasure, "set_short", "A giant emerald");
	call_other(treasure, "set_weight", 3);
	call_other(treasure, "set_value", 1000);
	move_object(treasure, balrog);
    }
}

spellfail(arg) {
    if (!balrog || !living(balrog)) return 0;
    if (arg && arg != "balrog") return 0;
    write ("Spells are useless against the the Balrog.\n");
    return 1;
}
