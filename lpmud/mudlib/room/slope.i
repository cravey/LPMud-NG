# 1 "room/slope.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/slope.c"

reset(arg) { if (!arg) { set_light( 1); short_desc =  	 "A slope"; long_desc =  	 "The forest gets light here, and slopes down to the west.\n"; dest_dir = ({ "room/orc_vall",  "west",  	 "room/forest2",  "east",  	 "room/forest3",  "south" }); }  }
# 8 "room/slope.c"
