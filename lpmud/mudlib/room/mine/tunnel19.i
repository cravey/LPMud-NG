# 1 "room/mine/tunnel19.c"
# 1 "room/mine/../room.h"
inherit "room/room";
























# 2 "room/mine/tunnel19.c"
reset(arg) { if (!arg) { set_light( 0); short_desc =  	 "Tunnel"; long_desc =  	 "The tunnel splits up in a fork forward.\n"; dest_dir = ({ "room/mine/tunnel17",  "south",  	 "room/mine/tunnel22",  "north" }); }  }
# 6 "room/mine/tunnel19.c"
