# 1 "players/morgar/mazeworld/minotaur.c"

# 1 "/usr/users/jseidman/lpmud/mudlib/room/std.h"







































































































































































# 2 "players/morgar/mazeworld/minotaur.c"

object minotaur;

extra_reset() {
    object treasure;
    if (!minotaur || !living(minotaur)) {
	minotaur = clone_object("obj/monster");
	call_other(minotaur, "set_name", "minotaur");
	call_other(minotaur, "set_alias", "bull");
	call_other(minotaur, "set_level", 15);
        call_other(minotaur, "set_hp", 150);
	call_other(minotaur, "set_short", "A minotaur");
	call_other(minotaur, "set_wc", 18);
	call_other(minotaur, "set_ac", 4);
	call_other(minotaur, "set_al", -300);
	call_other(minotaur, "set_aggressive", 1);
	move_object(minotaur, this_object());
    }
}




reset(arg) {     extra_reset(); if (arg) return; set_light( 0); }short() {    if (set_light(0))	return 
	 "In the pit of the minotaur";    return "dark room";}init() {    add_action("move"); add_verb( "up");    }move() {        call_other(this_player(), "move_player",  "up" + "#" +"players/morgar/mazeworld/maze10");    return 1;}long(str) {    if (set_light(0) == 0){       write("It is dark.\n");       return;    }        write(
	 "You are in a pit, which is obviously the home of a minotar.\n");    write("    The only obvious exit is " +  "up" + ".\n");}

realm() { return "maze"; }
