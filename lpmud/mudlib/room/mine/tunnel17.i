# 1 "room/mine/tunnel17.c"
# 1 "room/mine/../room.h"
inherit "room/room";
























# 2 "room/mine/tunnel17.c"
reset(arg) { if (!arg) { set_light( 0); short_desc =  	 "Tunnel"; long_desc =  	 "The tunnel into the mines.\n"; dest_dir = ({ "room/mine/tunnel19",  "north",  	 "room/mine/tunnel16",  "south",  	 "room/mine/tunnel18",  "west" }); }  }
# 7 "room/mine/tunnel17.c"
