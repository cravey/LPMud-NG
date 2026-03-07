# 1 "players/morgar/techworld/entry.c"

# 1 "/usr/users/jseidman/lpmud/mudlib/room/std.h"







































































































































































# 2 "players/morgar/techworld/entry.c"





object mirror;

reset(arg) { extra_reset(); if (arg) return; set_light( 1); }short() {    if (set_light(0))	return 
	 "An abandoned alley";    return "dark room";}init() {    add_action("move"); add_verb( "north");    }move() {        call_other(this_player(), "move_player",  "north" + "#" +"players/morgar/techworld/blockage");    return 1;}long(str) {    if (set_light(0) == 0){       write("It is dark.\n");       return;    }        write(
	 "You are at the southern end of an alley in what appears to be the\n" +
         "ruins of a once great technological city.  All around you skyscrapers\n" +
         "rise up to the sky.  However, the doors and garages around you are\n" +
         "all either boarded up or welded shut.  Large amounts of rubble clog\n" +
         "the alley.\n");    write("    The only obvious exit is " +  "north" + ".\n");}

extra_reset() {
    if (!mirror) {
        mirror = clone_object("players/morgar/mirror");
        call_other(mirror, "roomset", this_object());
        move_object(mirror, "players/morgar/hall3");
    }
}

realm() { return("tech"); }
