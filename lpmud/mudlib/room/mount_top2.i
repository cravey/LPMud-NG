# 1 "room/mount_top2.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/mount_top2.c"

reset(arg) { if (!arg) { set_light(1); short_desc =  	 "Plateau"; long_desc =  	 "You are on a large, open plateau on top of the mountain.\n"+    "The view is fantastic in all directions and the clouds\n"+    "that rush past above feels so close you could almost\n"+    "touch them. The air here is fresh and clean.\n"; dest_dir = ({ "room/mount_top",  "west" }); }  }
# 9 "room/mount_top2.c"

