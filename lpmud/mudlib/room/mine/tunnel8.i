# 1 "room/mine/tunnel8.c"
# 1 "room/mine/../room.h"
inherit "room/room";
























# 2 "room/mine/tunnel8.c"

reset(arg) { if (!arg) { set_light( 0); short_desc =  	 "Shaft"; long_desc =  	 "In a shaft going straight down.\n"; dest_dir = ({ "room/mine/tunnel3",  "up",  	 "room/mine/tunnel9",  "down" }); }  }
# 7 "room/mine/tunnel8.c"
