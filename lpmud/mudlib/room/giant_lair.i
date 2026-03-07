# 1 "room/giant_lair.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/giant_lair.c"

object giant;














reset(arg) { if (!arg) { set_light( 1); short_desc =  	 "Lair of the Giant"; long_desc =  	 "There are mountains all around you.\n"; dest_dir = ({ "room/giant_path",  "east",  	 "room/giant_conf",  "west" }); } if (!giant || !living(giant)) { giant = clone_object("obj/monster"); call_other(giant, "set_name", "giant"); call_other(giant, "set_level", 15); call_other(giant, "set_short", "A giant"); call_other(giant, "set_wc", 20); call_other(giant, "set_ac", 2); call_other(giant, "set_aggressive", 1); move_object(giant, this_object()); } }
# 23 "room/giant_lair.c"
