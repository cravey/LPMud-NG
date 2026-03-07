# 1 "room/mine/tunnel4.c"
# 1 "room/mine/../room.h"
inherit "room/room";
























# 2 "room/mine/tunnel4.c"

reset(arg) { if (!arg) { set_light( 0); short_desc =  	 "Tunnel"; long_desc =  	 "In the tunnel into the mines.\n"; dest_dir = ({ "room/mine/tunnel3",  "south",  	 "room/mine/tunnel5",  "north" }); }  }
# 7 "room/mine/tunnel4.c"
