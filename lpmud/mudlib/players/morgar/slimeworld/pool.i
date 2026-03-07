# 1 "players/morgar/slimeworld/pool.c"

# 1 "/usr/users/jseidman/lpmud/mudlib/room/std.h"







































































































































































# 2 "players/morgar/slimeworld/pool.c"


object slug1, slug2;

extra_reset() {
    if (!slug2 || !living(slug2)) {
	slug2 = clone_object("obj/monster");
	call_other(slug2, "set_name", "slime puddle");
	call_other(slug2, "set_alias", "puddle");
	call_other(slug2, "set_level", 14);
        call_other(slug2, "set_hp", 150);
	call_other(slug2, "set_short", "A puddle of slime");
	call_other(slug2, "set_wc", 12);
	call_other(slug2, "set_ac", 4);
	call_other(slug2, "set_al", -350);
	call_other(slug2, "set_aggressive", 1);
	move_object(slug2, this_object());
    }
    if (!slug1 || !living(slug1)) {
	slug1 = clone_object("obj/monster");
	call_other(slug1, "set_name", "slug");
	call_other(slug1, "set_alias", "slime slug");
	call_other(slug1, "set_level", 14);
        call_other(slug1, "set_hp", 150);
	call_other(slug1, "set_short", "A slime slug");
	call_other(slug1, "set_wc", 12);
	call_other(slug1, "set_ac", 4);
	call_other(slug1, "set_al", -350);
	call_other(slug1, "set_aggressive", 1);
	move_object(slug1, this_object());
    }
}




reset(arg) {     extra_reset(); if (arg) return; set_light( 1); }short() {    if (set_light(0))	return 
	 "Pit of the slime things";    return "dark room";}init() {    add_action("move1"); add_verb( "north");    add_action("move2"); add_verb( "east");    add_action("move3"); add_verb( "in");    }move1() {        call_other(this_player(), "move_player",  "north" + "#" + "players/morgar/slimeworld/entry");    return 1;}move2() {        call_other(this_player(), "move_player",  "east" + "#" + 
         "players/morgar/slimeworld/desolate_s");    return 1;}move3() {        call_other(this_player(), "move_player",  "in" + "#" + 
         "players/morgar/slimeworld/under");    return 1;}long(str) {    if (set_light(0) == 0){       write("It is dark.\n");       return;    }        write(
	 "You are at a huge lake of slime, filled with disgusting things.\n" +
         "From here it is possible to go down into the slime.\n");    write("There are three obvious exits, " +  "north" + ", " +  "east" +	  " and " +  "in" + ".\n");}


realm() {
    return("slime");
}
