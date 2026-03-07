# 1 "players/morgar/wizworld/entry.c"

# 1 "/usr/users/jseidman/lpmud/mudlib/room/std.h"







































































































































































# 2 "players/morgar/wizworld/entry.c"





object mirror, treasure1, treasure2, treasure3;

reset(arg) { extra_reset(); if (arg) return; set_light( 1); }short() {    if (set_light(0))	return 
	 "A magnificent entry chamber";    return "dark room";}init() {    add_action("move1"); add_verb( "north");    add_action("move2"); add_verb( "south");    }move1() {        call_other(this_player(), "move_player",  "north" + "#" + "players/morgar/wizworld/chamber");    return 1;}move2() {        call_other(this_player(), "move_player",  "south" + "#" + 
	 "players/morgar/wizworld/courtyard");    return 1;}long(str) {    if (set_light(0) == 0){       write("It is dark.\n");       return;    }        write(
	 "You are in a magnificent entry chamber, decorated in a baroque\n" +
         "manner.  Several exquisite paintings line the walls, and there is\n" +
         "a fresco on the ceiling.  In addition, the whole is carpeted with\n" +
         "an expensive carpet.\n" +
         "The meeting chamber is to the north, and a courtyard is to the south.\n");    write("There are two obvious exits, " +  "north" + " and " +  "south" + ".\n");}

extra_reset() {
    if (!mirror) {
        mirror = clone_object("players/morgar/mirror");
        call_other(mirror, "roomset", this_object());
        move_object(mirror, "players/morgar/hall2");
    }
    
    if (!present("nature painting")) {
        treasure1 = clone_object("obj/treasure");
        call_other(treasure1, "set_id", "nature painting");
        call_other(treasure1, "set_alias", "painting");
        call_other(treasure1, "set_short", "A painting of the field outside of the tower of the gates");
        call_other(treasure1, "set_weight", 100);
        call_other(treasure1, "set_value", 1500);
        move_object(treasure1, this_object());
    }
    if (!present("storm painting")) {
        treasure2 = clone_object("obj/treasure");
        call_other(treasure2, "set_id", "storm painting");
        call_other(treasure2, "set_alias", "painting");
        call_other(treasure2, "set_short", "A painting of a thunderstorm");
        call_other(treasure2, "set_weight", 100);
        call_other(treasure2, "set_value", 2000);
        move_object(treasure2, this_object());
    }
    if (!present("large painting")) {
        treasure3 = clone_object("obj/treasure");
        call_other(treasure3, "set_id", "large painting");
        call_other(treasure3, "set_alias", "painting");
        call_other(treasure3, "set_short", "A huge painting of a battle scene");
        call_other(treasure3, "set_weight", 100);
        call_other(treasure3, "set_value", 2500);
        move_object(treasure3, this_object());
    }
}

realm() {
    return("wizard");
}
