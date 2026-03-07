# 1 "room/mine/tunnel27.c"
# 1 "room/mine/../room.h"
inherit "room/room";
























# 2 "room/mine/tunnel27.c"
reset(arg) { if (!arg) { set_light( 0); short_desc =  	 "Dead end"; long_desc =  	 "End of tunnel.\n"; dest_dir = ({ "room/mine/tunnel26",  "south" }); }  }
# 5 "room/mine/tunnel27.c"
