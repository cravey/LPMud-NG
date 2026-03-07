# 1 "room/mine/tunnel22.c"
# 1 "room/mine/../room.h"
inherit "room/room";
























# 2 "room/mine/tunnel22.c"
reset(arg) { if (!arg) { set_light( 0); short_desc =  	 "Tunnel"; long_desc =  	 "Tunnel fork.\n"; dest_dir = ({ "room/mine/tunnel19",  "south",  	 "room/mine/tunnel21",  "west",  	 "room/mine/tunnel23",  "east" }); }  }
# 7 "room/mine/tunnel22.c"
