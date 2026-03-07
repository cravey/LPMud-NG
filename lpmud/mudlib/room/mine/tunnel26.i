# 1 "room/mine/tunnel26.c"
# 1 "room/mine/../room.h"
inherit "room/room";
























# 2 "room/mine/tunnel26.c"
reset(arg) { if (!arg) { set_light( 0); short_desc =  	 "Tunnel"; long_desc =  	 "The tunnel slopes steeply down a hole here.\n"; dest_dir = ({ "room/mine/tunnel25",  "south",  	 "room/mine/tunnel27",  "north",  	 "room/mine/tunnel28",  "east" }); }  }
# 7 "room/mine/tunnel26.c"
