# 1 "room/forest3.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/forest3.c"

reset(arg) { if (!arg) { set_light( 1); short_desc =  	 "Deep forest"; long_desc =  	 "You are in the deep forest.\n"; dest_dir = ({ "room/slope",  "north",  	 "room/forest4",  "south" }); }  }
# 7 "room/forest3.c"
