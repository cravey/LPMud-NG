# 1 "room/forest4.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/forest4.c"

reset(arg) { if (!arg) { set_light( 1); short_desc =  	 "Deep forest"; long_desc =  	 "You are in the deep forest.\n"; dest_dir = ({ "room/forest3",  "north",  	 "room/forest5",  "west",  	 "room/forest6",  "east",  	 "room/forest7",  "south" }); }  }
# 9 "room/forest4.c"
