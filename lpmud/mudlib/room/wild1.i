# 1 "room/wild1.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/wild1.c"




reset(arg) { if (!arg) { set_light( 1); short_desc =  	 "Wilderness"; long_desc =  	 "You are in the wilderness outside the village.\n" + 	 "There is a big forest to the west.\n"; dest_dir = ({ "room/hump",  "east",  	 "room/forest1",  "west" }); } no_castle_flag = 1; }
# 11 "room/wild1.c"
