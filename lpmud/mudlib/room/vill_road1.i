# 1 "room/vill_road1.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/vill_road1.c"



reset(arg) { if (!arg) { set_light( 1); short_desc =  	   "Village road"; long_desc =  "A long road going east through the village. The road narrows to a\n" + "track to the west. There is an alley to the north and the south.\n"; dest_dir = ({ "room/vill_track", "west",  	   "room/yard", "north",  	   "room/narr_alley", "south",  	   "room/vill_road2", "east" }); } no_castle_flag = 1; }
# 12 "room/vill_road1.c"
