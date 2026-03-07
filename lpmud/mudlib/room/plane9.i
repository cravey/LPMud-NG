# 1 "room/plane9.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/plane9.c"

object stethoscope;
object frog;

extra_reset() {
    if (!stethoscope || environment(stethoscope) != this_object()) {
	stethoscope = clone_object("obj/stethoscope");
	move_object(stethoscope, this_object());
    }
    if (!frog || !living(frog)) {
	object crown;
	frog = clone_object("obj/monster");
	call_other(frog, "set_name", "frog");
	call_other(frog, "set_short", "A cute little frog");
	call_other(frog, "set_wc", 4);
	call_other(frog, "set_level", 1);
	call_other(frog, "set_frog", 1);
	move_object(frog, this_object());
	crown = clone_object("obj/treasure");
	call_other(crown, "set_id", "crown");
	call_other(crown, "set_value", 30);
	call_other(crown, "set_short", "An iron crown");
	move_object(crown, frog);
    }
}





reset(arg) { if (!arg) { set_light( 1); short_desc =  	 "A large open plain"; long_desc =  	 "A large open plain. There is a river to the east,\nand some kind of building to the west\n"; dest_dir = ({ "room/ruin",  "west" }); } extra_reset(); }
# 36 "room/plane9.c"
