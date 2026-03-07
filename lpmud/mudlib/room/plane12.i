# 1 "room/plane12.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/plane12.c"

reset(arg) { if (!arg) { set_light( 1); short_desc =  	   "A large open plain"; long_desc =  	   "A large open plain. There is a forest to the west\n"; dest_dir = ({ "room/deep_forest1",  "west",  	   "room/plane11",  "east",  	   "room/plane10",  "south" }); }  }
# 8 "room/plane12.c"
