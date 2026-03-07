# 1 "room/ravine.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/ravine.c"

reset(arg) { if (!arg) { set_light( 1); short_desc =  	 "Ravine"; long_desc =  	 "You are in a ravine between mountains. It seems to be possible\n"+ 	 "to go up from here.\n"; dest_dir = ({ "room/mount_pass",  "down",  	 "room/mount_top",  "up" }); }  }
# 8 "room/ravine.c"

