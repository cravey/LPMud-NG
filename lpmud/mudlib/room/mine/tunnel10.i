# 1 "room/mine/tunnel10.c"
# 1 "room/mine/../room.h"
inherit "room/room";
























# 2 "room/mine/tunnel10.c"
reset(arg) { if (!arg) { set_light( 0); short_desc =  	 "Tunnel"; long_desc =  	 "In the tunnel into the mines.\n"; dest_dir = ({ "room/mine/tunnel9",  "east",  	 "room/mine/tunnel11",  "west" }); }  }
# 6 "room/mine/tunnel10.c"
