#include "std.h"

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

#undef EXTRA_RESET
#define EXTRA_RESET\
    extra_reset();

THREE_EXIT("room/plane4", "west",
         "players/crimson/entry", "south",
         "players/crimson/riverbank", "east",
	 "A large open plain",
         "You are on a narrow prairie footpath. To the south you see an odd\n" +
         "looking old house. The prairie stretches out to the west and north\n"+
         "you can see the river. Eastward, there is a grove of trees.\n", 1)
