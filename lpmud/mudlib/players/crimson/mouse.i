# 1 "players/crimson/mouse.c"
# 1 "/home/lpmud/mudlib/room/std.h"







































































































































































# 2 "players/crimson/mouse.c"

object fieldmouse;

extra_reset() {
    if (!fieldmouse || !living(fieldmouse)) {
	object seed;
	fieldmouse = clone_object("obj/monster");
	call_other(fieldmouse, "set_name", "fieldmouse");
        call_other(fieldmouse, "set_alias", "mouse");
	call_other(fieldmouse, "set_short", "A sweet little fieldmouse");
	call_other(fieldmouse, "set_wc", 4);
        call_other(fieldmouse, "set_whimpy");
	call_other(fieldmouse, "set_level", 1);
        call_other(fieldmouse, "set_al", 150);
        call_other(fieldmouse, "set_move_at_reset");
        move_object(fieldmouse, "room/plane4");
	seed = clone_object("obj/treasure");
	call_other(seed, "set_id", "seed");
	call_other(seed, "set_value", 30);
	call_other(seed, "set_short", "A tiny seed");
	move_object(seed, fieldmouse);
    }
}





reset(arg) { extra_reset(); if (arg) return; set_light( 1); } short() { if (set_light(0)) return  	 "A large open plain"; return "dark room"; } init() { add_action("move1",  "west"); add_action("move2",  "south"); add_action("move3",  "east");  } move1() {  call_other(this_player(), "move_player",  "west" + "#" + "room/plane4"); return 1; } move2() {  call_other(this_player(), "move_player",  "south" + "#" +           "players/crimson/entry"); return 1; } move3() {  call_other(this_player(), "move_player",  "east" + "#" +           "players/crimson/riverbank"); return 1; } long(str) { if (set_light(0) == 0){ write("It is dark.\n"); return; }  write(          "You are on a narrow prairie footpath. To the south you see an odd\n" +          "looking old house. The prairie stretches out to the west and north\n"+          "you can see the river. Eastward, there is a grove of trees.\n"); write("There are three obvious exits, " +  "west" + ", " +  "south" + " and " +  "east" + ".\n"); }
# 38 "players/crimson/mouse.c"
