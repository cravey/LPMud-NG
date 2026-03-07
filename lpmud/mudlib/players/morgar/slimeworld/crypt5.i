# 1 "players/morgar/slimeworld/crypt5.c"

# 1 "/usr/users/jseidman/lpmud/mudlib/room/std.h"







































































































































































# 2 "players/morgar/slimeworld/crypt5.c"

object vampire;

extra_reset() {
    object treasure;
    if (!vampire || !living(vampire)) {
	vampire = clone_object("obj/monster");
	call_other(vampire, "set_name", "vampire");
	call_other(vampire, "set_alias", "undead");
	call_other(vampire, "set_level", 17);
	call_other(vampire, "set_short", "A vampire");
	call_other(vampire, "set_wc", 20);
	call_other(vampire, "set_ac", 7);
	call_other(vampire, "set_al", -500);
	call_other(vampire, "set_aggressive", 1);
	move_object(vampire, this_object());
	treasure = clone_object("obj/treasure");
	call_other(treasure, "set_id", "ring");
	call_other(treasure, "set_alias", "gold ring");
	call_other(treasure, "set_short", "A gold ring");
	call_other(treasure, "set_weight", 1);
	call_other(treasure, "set_value", 2000);
	move_object(treasure, vampire);
    }
}




reset(arg) {     extra_reset(); if (arg) return; set_light( 0); }short() {    if (set_light(0))	return 
	 "In a dank, dark crypt";    return "dark room";}init() {    add_action("move"); add_verb( "south");    }move() {        call_other(this_player(), "move_player",  "south" + "#" +"players/morgar/slimeworld/crypt4");    return 1;}long(str) {    if (set_light(0) == 0){       write("It is dark.\n");       return;    }        write(
	 "You are in the crypt under the ruined temple.  There are several\n" +
         "empty coffins lying around.\n");    write("    The only obvious exit is " +  "south" + ".\n");}

realm() {
    return("slime");
}
