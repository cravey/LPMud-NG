# 1 "players/morgar/wizworld/chamber.c"

# 1 "/usr/users/jseidman/lpmud/mudlib/room/std.h"







































































































































































# 2 "players/morgar/wizworld/chamber.c"





object treasure1, treasure2, treasure3;

reset(arg) { extra_reset(); if (arg) return; set_light( 1); }short() {    if (set_light(0))	return 
	 "The council chamber";    return "dark room";}init() {    add_action("move1"); add_verb( "south");    add_action("move2"); add_verb( "east");    add_action("move3"); add_verb( "west");    add_action("move4"); add_verb( "north");    }move1() {        call_other(this_player(), "move_player",  "south" + "#" + "players/morgar/wizworld/entry");    return 1;}move2() {        call_other(this_player(), "move_player",  "east" + "#" + 
         "players/morgar/wizworld/magicroom");    return 1;}move3() {        call_other(this_player(), "move_player",  "west" + "#" + 
	 "players/morgar/wizworld/chaos");    return 1;}move4() {        call_other(this_player(), "move_player",  "north" + "#" + 
	 "players/morgar/wizworld/maproom");    return 1;}long(str) {    if (set_light(0) == 0){       write("It is dark.\n");       return;    }        write(
	 "You are in the council chamber of the circle of wizards, where they\n" +
         "hold their meetings.  It is done in a style and grandeur that truly\n" +
         "befits the circle of wizards.  As in the entry, the walls are adorned\n" +
         "with paintings, and the ceiling is done in a fresco that, if anything,\n" +
         "exceeds the one in the entry.\n");    write("There are four obvious exits, " +  "south" + ", " +  "east" + ", " +	   "west" + " and " +  "north" + ".\n");}

extra_reset() {
    if (!present("ocean painting")) {
        treasure1 = clone_object("obj/treasure");
        call_other(treasure1, "set_id", "ocean painting");
        call_other(treasure1, "set_alias", "painting");
        call_other(treasure1, "set_short", "A painting of the sea, as seen from the village dock");
        call_other(treasure1, "set_weight", 100);
        call_other(treasure1, "set_value", 2000);
        move_object(treasure1, this_object());
    }
    if (!present("tower painting")) {
        treasure2 = clone_object("obj/treasure");
        call_other(treasure2, "set_id", "tower painting");
        call_other(treasure2, "set_alias", "painting");
        call_other(treasure2, "set_short", "A painting of the tower of the gates");
        call_other(treasure2, "set_weight", 100);
        call_other(treasure2, "set_value", 2500);
        move_object(treasure2, this_object());
    }
    if (!present("huge painting")) {
        treasure3 = clone_object("obj/treasure");
        call_other(treasure3, "set_id", "huge painting");
        call_other(treasure3, "set_alias", "painting");
        call_other(treasure3, "set_short", "A huge painting of the creation of the world");
        call_other(treasure3, "set_weight", 100);
        call_other(treasure3, "set_value", 3500);
        move_object(treasure3, this_object());
    }
}

realm() {
    return("wizard");
}
