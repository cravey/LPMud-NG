# 1 "room/giant_path.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/giant_path.c"

reset(arg) { if (!arg) { set_light( 1); short_desc =  	 "A path"; long_desc =  	 "You are on a path going in east/west direction. There are some\n" + 	 "VERY big footsteps here.\n"; dest_dir = ({ "room/big_tree",  "east",  	 "room/giant_lair",  "west" }); }  }
# 8 "room/giant_path.c"

