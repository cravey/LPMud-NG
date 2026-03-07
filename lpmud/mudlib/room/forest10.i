# 1 "room/forest10.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/forest10.c"

reset(arg) { if (!arg) { set_light( 1); short_desc =  	 "Deep forest"; long_desc =  	 "You are in the deep forest.\n"; dest_dir = ({ "room/forest9",  "west",  	 "room/forest7",  "east" }); }  }
# 7 "room/forest10.c"
