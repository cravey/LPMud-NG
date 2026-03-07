# 1 "players/morgar/mazeworld/entry.c"

# 1 "/usr/users/jseidman/lpmud/mudlib/room/std.h"







































































































































































# 2 "players/morgar/mazeworld/entry.c"





object mirror;

reset(arg) { extra_reset(); if (arg) return; set_light( 1); }short() {    if (set_light(0))	return 
	 "Crossing paths";    return "dark room";}init() {    add_action("move1"); add_verb( "north");    add_action("move2"); add_verb( "south");    add_action("move3"); add_verb( "east");    add_action("move4"); add_verb( "west");    }move1() {        call_other(this_player(), "move_player",  "north" + "#" + "players/morgar/mazeworld/maze1");    return 1;}move2() {        call_other(this_player(), "move_player",  "south" + "#" + 
         "players/morgar/mazeworld/maze2");    return 1;}move3() {        call_other(this_player(), "move_player",  "east" + "#" + 
         "players/morgar/mazeworld/maze3");    return 1;}move4() {        call_other(this_player(), "move_player",  "west" + "#" + 
         "players/morgar/mazeworld/maze4");    return 1;}long(str) {    if (set_light(0) == 0){       write("It is dark.\n");       return;    }        write(
	 "You are at a junction between a north-south passageway and an\n" +
         "east-west passageway.  The entire area is carved from stone, and\n" +
         "is lit by some strange magic that causes the air itself to glow.\n" +
         "The corridors are about nine feet high and twelve feet across,\n" +
         "and everything is constructed with a perfectly rectangular rectangular\n" +
         "Geometry.  The walls are a black basalt.\n");    write("There are four obvious exits, " +  "north" + ", " +  "south" + ", " +	   "east" + " and " +  "west" + ".\n");}

extra_reset() {
    if (!mirror) {
        mirror = clone_object("players/morgar/mirror");
        call_other(mirror, "roomset", this_object());
        move_object(mirror, "players/morgar/hallend");
    }
}

realm() {
    return("maze");
}
