# 1 "players/morgar/slimeworld/crypt4.c"

# 1 "/usr/users/jseidman/lpmud/mudlib/room/std.h"







































































































































































# 2 "players/morgar/slimeworld/crypt4.c"

object skeleton, zombie;

extra_reset() {
    object treasure;
    if (!zombie || !living(zombie)) {
	zombie = clone_object("obj/monster");
	call_other(zombie, "set_name", "zombie");
	call_other(zombie, "set_alias", "undead");
	call_other(zombie, "set_level", 8);
        call_other(zombie, "set_hp", 70);
	call_other(zombie, "set_short", "A zombie");
	call_other(zombie, "set_wc", 7);
	call_other(zombie, "set_ac", 5);
	call_other(zombie, "set_al", -200);
	call_other(zombie, "set_aggressive", 1);
	move_object(zombie, this_object());
	treasure = clone_object("obj/treasure");
	call_other(treasure, "set_id", "chain");
	call_other(treasure, "set_alias", "gold chain");
	call_other(treasure, "set_short", "A gold chain");
	call_other(treasure, "set_weight", 1);
	call_other(treasure, "set_value", 200);
	move_object(treasure, zombie);
    }
    if (!skeleton || !living(skeleton)) {
        skeleton = clone_object("obj/monster");
        call_other(skeleton, "set_name", "skeleton");
        call_other(skeleton, "set_alias", "undead");
        call_other(skeleton, "set_level", 8);
        call_other(skeleton, "set_hp", 70);
        call_other(skeleton, "set_short", "A skeleton");
        call_other(skeleton, "set_wc", 7);
        call_other(skeleton, "set_ac", 5);
        call_other(skeleton, "set_al", -200);
        call_other(skeleton, "set_aggressive", 1);
        move_object(skeleton, this_object());
        treasure = clone_object("obj/treasure");
        call_other(treasure, "set_id", "ring");
        call_other(treasure, "set_alias", "gold ring");
        call_other(treasure, "set_short", "A gold ring");
        call_other(treasure, "set_weight", 1);
        call_other(treasure, "set_value", 200);
        move_object(treasure, skeleton);
    }
}




reset(arg) {     extra_reset(); if (arg) return; set_light( 0); }short() {    if (set_light(0))	return 
	 "In a dank, dark crypt";    return "dark room";}init() {    add_action("move1"); add_verb( "south");    add_action("move2"); add_verb( "north");    }move1() {        call_other(this_player(), "move_player",  "south" + "#" + "players/morgar/slimeworld/crypt2");    return 1;}move2() {        call_other(this_player(), "move_player",  "north" + "#" + 
         "players/morgar/slimeworld/crypt5");    return 1;}long(str) {    if (set_light(0) == 0){       write("It is dark.\n");       return;    }        write(
	 "You are in the crypt under the ruined temple.  There are several\n" +
         "empty coffins lying around.\n");    write("There are two obvious exits, " +  "south" + " and " +  "north" + ".\n");}

realm() {
    return("slime");
}
