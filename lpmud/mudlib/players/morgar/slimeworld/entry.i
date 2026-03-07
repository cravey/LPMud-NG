# 1 "players/morgar/slimeworld/entry.c"

# 1 "/usr/users/jseidman/lpmud/mudlib/room/std.h"







































































































































































# 2 "players/morgar/slimeworld/entry.c"





object mirror;

reset(arg) { extra_reset(); if (arg) return; set_light( 1); }short() {    if (set_light(0))	return 
	 "A sloping path";    return "dark room";}init() {    add_action("move1"); add_verb( "north");    add_action("move2"); add_verb( "south");    add_action("move3"); add_verb( "east");    add_action("move4"); add_verb( "west");    }move1() {        call_other(this_player(), "move_player",  "north" + "#" + "players/morgar/slimeworld/mound");    return 1;}move2() {        call_other(this_player(), "move_player",  "south" + "#" + 
	 "players/morgar/slimeworld/pool");    return 1;}move3() {        call_other(this_player(), "move_player",  "east" + "#" + 
	 "players/morgar/slimeworld/desolate");    return 1;}move4() {        call_other(this_player(), "move_player",  "west" + "#" + 
	 "players/morgar/slimeworld/pillars");    return 1;}long(str) {    if (set_light(0) == 0){       write("It is dark.\n");       return;    }        write(
	 "You are on a path sloping down to the south.  There are large\n" +
	 "numbers of slimy trails here.\n");    write("There are four obvious exits, " +  "north" + ", " +  "south" + ", " +	   "east" + " and " +  "west" + ".\n");}

extra_reset() {
    if (!mirror) {
        mirror = clone_object("players/morgar/mirror");
        call_other(mirror, "roomset", this_object());
        move_object(mirror, "players/morgar/hall");
    }
}

realm() {
    return("slime");
}
