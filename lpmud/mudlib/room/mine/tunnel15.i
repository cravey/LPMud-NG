# 1 "room/mine/tunnel15.c"
# 1 "room/mine/../room.h"
inherit "room/room";
























# 2 "room/mine/tunnel15.c"
reset(arg) { if (!arg) { set_light( 0); short_desc =  	 "Tunnel"; long_desc =  	 "In the tunnel into the mines.\n"; dest_dir = ({ "room/mine/tunnel16",  "east",  	 "room/mine/tunnel14",  "west" }); }  }
# 6 "room/mine/tunnel15.c"
