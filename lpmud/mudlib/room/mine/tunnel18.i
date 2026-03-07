# 1 "room/mine/tunnel18.c"
# 1 "room/mine/../room.h"
inherit "room/room";
























# 2 "room/mine/tunnel18.c"
reset(arg) { if (!arg) { set_light( 0); short_desc =  	 "Dead end"; long_desc =  	 "In the tunnel into the mines.\n"; dest_dir = ({ "room/mine/tunnel17",  "east" }); }  }
# 5 "room/mine/tunnel18.c"
