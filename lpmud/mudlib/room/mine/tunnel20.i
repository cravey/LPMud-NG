# 1 "room/mine/tunnel20.c"
# 1 "room/mine/../room.h"
inherit "room/room";
























# 2 "room/mine/tunnel20.c"
reset(arg) { if (!arg) { set_light( 0); short_desc =  	 "Dead end"; long_desc =  	 "Dead end.\n"; dest_dir = ({ "room/mine/tunnel21",  "east" }); }  }
# 5 "room/mine/tunnel20.c"
