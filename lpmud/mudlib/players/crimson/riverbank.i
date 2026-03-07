# 1 "players/crimson/riverbank.c"
# 1 "/home/lpmud/mudlib/room/std.h"







































































































































































# 2 "players/crimson/riverbank.c"

object fieldmouse;

extra_reset() {
    if (!fieldmouse || !living(fieldmouse)) {
	object seed;
	fieldmouse = clone_object("obj/monster");
	call_other(fieldmouse, "set_name", "fieldmouse");
        call_other(fieldmouse, "set_alias", "mouse");
	call_other(fieldmouse, "set_short", "A sweet little fieldmouse");
	call_other(fieldmouse, "set_wc", 4);
	call_other(fieldmouse, "set_level", 1);
        call_other(fieldmouse, "set_al", 150);
        call_other(fieldmouse, "set_whimpy");
        call_other(fieldmouse, "set_move_at_reset");
        move_object(fieldmouse, "players/crimson/mouse");
	seed = clone_object("obj/treasure");
	call_other(seed, "set_id", "seed");
	call_other(seed, "set_value", 30);
	call_other(seed, "set_short", "A tiny seed");
	move_object(seed, fieldmouse);
    }
}





reset(arg) { extra_reset(); if (arg) return; set_light( 1); } short() { if (set_light(0)) return           "A grove of trees"; return "dark room"; } init() { add_action("move",  "west");  } move() {  call_other(this_player(), "move_player",  "west" + "#" +"players/crimson/mouse"); return 1; } long(str) { if (set_light(0) == 0){ write("It is dark.\n"); return; }  write(          "The trail winds and twists here. Green lush trees hover overhead.\n"+          "You are now walking parallel to the river. The willows are in bloom\n"+          "and you occasionally hear the croaking of a bullfrog.\n"); write("    The only obvious exit is " +  "west" + ".\n"); }
# 36 "players/crimson/riverbank.c"
