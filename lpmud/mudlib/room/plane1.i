# 1 "room/plane1.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/plane1.c"
object wolf;
















reset(arg) { if (!arg) { set_light( 1); short_desc =  	 "A large open plain"; long_desc =  	 "A large open plain, extending to north and south.\n"; dest_dir = ({ "room/clearing",  "south",  	 "room/plane2",  "north" }); } if (!wolf) { wolf = clone_object("obj/monster"); call_other(wolf, "set_name", "wolf"); call_other(wolf, "set_level", 3); call_other(wolf, "set_short", "A wolf"); call_other(wolf, "set_long", "A grey wolf running around. It has\n" + "some big dangerous teeth.\n"); call_other(wolf, "set_wc", 7); call_other(wolf, "set_move_at_reset"); call_other(wolf, "set_whimpy"); move_object(wolf, "room/ruin"); } }
# 24 "room/plane1.c"
