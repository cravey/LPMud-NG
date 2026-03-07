# 1 "room/plane2.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/plane2.c"

reset(arg) { if (!arg) { set_light( 1); short_desc =  	  "A large open plain"; long_desc =  	  "A large open plain, extending in all directions.\n"; dest_dir = ({ "room/plane1",  "south",  	  "room/plane3",  "north",  	  "room/plane4",  "east",  	  "room/plane5",  "west" }); }  }
# 9 "room/plane2.c"
