# 1 "room/clearing.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/clearing.c"

reset(arg) { if (!arg) { set_light( 1); short_desc =  	   "Clearing"; long_desc =  	   "A small clearing. There are trees all around you.\n" + 	   "However, the trees are sparse to the north.\n"; dest_dir = ({ "room/forest1",  "east",  	   "room/forest2",  "west",  	   "room/plane1",  "north" }); }  }
# 9 "room/clearing.c"
