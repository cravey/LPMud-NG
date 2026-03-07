# 1 "room/plane7.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/plane7.c"

reset(arg) { if (!arg) { set_light( 	  1); short_desc =  	  "A large open plain"; long_desc =  	  "A large open plain. There is a big tree to the west.\n"; dest_dir = ({ "room/plane5",  "south",  	  "room/plane10",  "north",  	  "room/plane3",  "east",  	  "room/big_tree",  "west" }); }  }
# 10 "room/plane7.c"
