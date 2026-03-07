# 1 "room/mine/tunnel28.c"
# 1 "room/mine/../room.h"
inherit "room/room";
























# 2 "room/mine/tunnel28.c"
reset(arg) { if (!arg) { set_light( 0); short_desc =  	 "Tunnel"; long_desc =  	 "Tunnel into the mines.\n"; dest_dir = ({ "room/mine/tunnel26",  "west",  	 "room/mine/tunnel29",  "east" }); }  }
# 6 "room/mine/tunnel28.c"
