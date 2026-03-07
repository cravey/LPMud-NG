# 1 "room/forest9.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/forest9.c"

reset(arg) { if (!arg) { set_light( 1); short_desc =  	 "Deep forest"; long_desc =  	 "You are in the deep forest.\n"; dest_dir = ({ "room/forest8",  "north",  	 "room/forest10",  "east",  	 "room/forest11",  "west" }); }  }
# 8 "room/forest9.c"
