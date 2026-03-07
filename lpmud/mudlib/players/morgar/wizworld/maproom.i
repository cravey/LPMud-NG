# 1 "players/morgar/wizworld/maproom.c"

# 1 "/usr/users/jseidman/lpmud/mudlib/room/std.h"







































































































































































# 2 "players/morgar/wizworld/maproom.c"




object map;

reset(arg) { extra_reset(); if (arg) return; set_light( 1); }short() {    if (set_light(0))	return 
	 "The map room";    return "dark room";}init() {    add_action("move"); add_verb( "south");    }move() {        call_other(this_player(), "move_player",  "south" + "#" +"players/morgar/wizworld/chamber");    return 1;}long(str) {    if (set_light(0) == 0){       write("It is dark.\n");       return;    }        write(
	 "You are in a room that is intended to contain a map of the entire\n" +
         "world.  However, due to the extensive creation by the various wizards,\n" +
         "the map is often far from complete and sometimes in error.\n");    write("    The only obvious exit is " +  "south" + ".\n");}

extra_reset() {
    if (!present("world map")) {
        map = clone_object("obj/treasure");
        call_other(map, "set_id", "world map");
        call_other(map, "set_alias", "map");
        call_other(map, "set_short", "A map of the world");
        call_other(map, "set_weight", 30);
        call_other(map, "set_value", 2000);
        call_other(map, "set_long",
"The map looks roughly like this:\n\n" +
"      + *\n" +
"      + +\n" +
"      ++++\n" +
"       +++\n" +
"   +++++++++\n" +
"       |||\n" +
"       +++*      *+ +\n" +
" +      |         |+++  +   *\n" +
" +      +         | |   |   |\n" +
" +-+-+--+-+-*-+-+-+-+---+-+-+-*\n" +
"   +    |           |   |\n" +
" ++++   +*          |   +\n" +
" | |    +          +*++\n" +
" +++    +           +\n" +
" +     +++\n" +
" *      *\n\n" +
"The *'s refer to other maps, most of which are not here\n");
        move_object(map, this_object());
    }
}

realm() { return("wizard"); }
