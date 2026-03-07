# 1 "room/mount_top.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/mount_top.c"

reset(arg) { if (!arg) { set_light( 1); short_desc =  	 "Top of mountain"; long_desc =  	 "You are on top of a mountain. There is a small plateau to the\n"+ 	 "east.\n"; dest_dir = ({ "room/ravine",  "down",  	 "room/mount_top2",  "east" }); }  }
# 8 "room/mount_top.c"
