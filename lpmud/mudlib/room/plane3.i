# 1 "room/plane3.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/plane3.c"

reset(arg) { if (!arg) { set_light( 	  1); short_desc =  	  "A large open plain"; long_desc =  	  "A large open plain. There are some kind of building to the east.\n"; dest_dir = ({ "room/plane2",  "south",  	  "room/plane6",  "north",  	  "room/ruin",  "east",  	  "room/plane7",  "west" }); }  }
# 10 "room/plane3.c"
