# 1 "players/morgar/slimeworld/under.c"

# 1 "/usr/users/jseidman/lpmud/mudlib/room/std.h"







































































































































































# 2 "players/morgar/slimeworld/under.c"

object kraken;

extra_reset() {
    object treasure;
    if (!kraken || !living(kraken)) {
	kraken = clone_object("obj/monster");
	call_other(kraken, "set_name", "kraken");
	call_other(kraken, "set_alias", "squid");
	call_other(kraken, "set_level", 19);
        call_other(kraken, "set_hp", 350);
	call_other(kraken, "set_short", "A kraken");
	call_other(kraken, "set_wc", 28);
	call_other(kraken, "set_ac", 7);
	call_other(kraken, "set_al", -400);
	call_other(kraken, "set_aggressive", 1);
        call_other(kraken, "set_chance",25);
        call_other(kraken, "set_spell_mess1",
                   "The kraken attacks with its tentacles.");
        call_other(kraken, "set_spell_mess2",
                   "The kraken wraps its tentacles around you, breaking bones.");
        call_other(kraken, "set_spell_dam",40);
	move_object(kraken, this_object());
	treasure = clone_object("obj/treasure");
	call_other(treasure, "set_id", "diamond");
	call_other(treasure, "set_alias", "gem");
	call_other(treasure, "set_short", "A giant diamond");
	call_other(treasure, "set_weight", 3);
	call_other(treasure, "set_value", 1000);
	move_object(treasure, kraken);
    }
}




reset(arg) {     extra_reset(); if (arg) return; set_light( 1); }short() {    if (set_light(0))	return 
	 "In the pool of slime things";    return "dark room";}init() {    add_action("move"); add_verb( "up");    }move() {        call_other(this_player(), "move_player",  "up" + "#" +"players/morgar/slimeworld/pool");    return 1;}long(str) {    if (set_light(0) == 0){       write("It is dark.\n");       return;    }        write(
	 "You are in a huge pool of slime, filled with disgusting things.\n" +
         "If feels as if there are things swimming around in the slime.\n");    write("    The only obvious exit is " +  "up" + ".\n");}

realm() { return "slime"; }
