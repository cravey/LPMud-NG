# 1 "room/mine/tunnel25.c"
# 1 "room/mine/../room.h"
inherit "room/room";
























# 2 "room/mine/tunnel25.c"
reset(arg) { if (!arg) { set_light( 0); short_desc =  	 "Tunnel"; long_desc =  	 "The tunnel slopes steeply down a hole here.\n"; dest_dir = ({ "room/mine/tunnel24",  "up",  	 "room/mine/tunnel26",  "north" }); }  }
# 6 "room/mine/tunnel25.c"
