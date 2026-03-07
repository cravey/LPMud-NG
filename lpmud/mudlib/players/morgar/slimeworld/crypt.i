# 1 "players/morgar/slimeworld/crypt.c"

# 1 "/usr/users/jseidman/lpmud/mudlib/room/std.h"







































































































































































# 2 "players/morgar/slimeworld/crypt.c"

object skeleton;

extra_reset() {
    object treasure;
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
	call_other(treasure, "set_id", "diamond");
	call_other(treasure, "set_alias", "gem");
	call_other(treasure, "set_short", "A small diamond");
	call_other(treasure, "set_weight", 1);
	call_other(treasure, "set_value", 200);
	move_object(treasure, skeleton);
    }
}




reset(arg) {     extra_reset(); if (arg) return; set_light( 0); }short() {    if (set_light(0))	return 
	 "In a dank, dark crypt";    return "dark room";}init() {    add_action("move1"); add_verb( "up");    add_action("move2"); add_verb( "north");    add_action("move3"); add_verb( "south");    }move1() {        call_other(this_player(), "move_player",  "up" + "#" + "players/morgar/slimeworld/ruins");    return 1;}move2() {        call_other(this_player(), "move_player",  "north" + "#" + 
         "players/morgar/slimeworld/crypt2");    return 1;}move3() {        call_other(this_player(), "move_player",  "south" + "#" + 
         "players/morgar/slimeworld/crypt3");    return 1;}long(str) {    if (set_light(0) == 0){       write("It is dark.\n");       return;    }        write(
	 "You are in the crypt under the ruined temple.  There are several\n" +
         "shattered and empty coffins lying around.  A stone staircase leads up.\n");    write("There are three obvious exits, " +  "up" + ", " +  "north" +	  " and " +  "south" + ".\n");}

realm() {
    return("slime");
}
