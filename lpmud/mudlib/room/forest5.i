# 1 "room/forest5.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/forest5.c"

reset(arg) { if (!arg) { set_light( 1); short_desc =  	 "Deep forest"; long_desc =  	 "You are in the deep forest.\n"; dest_dir = ({ "room/forest8",  "west",  	 "room/forest4",  "east" }); }  }
# 7 "room/forest5.c"
