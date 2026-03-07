# 1 "room/eastroad3.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/eastroad3.c"
reset(arg) { if (!arg) { set_light( 1); short_desc =  "East road"; long_desc =  "East road runs north-south.\n"+ "Sun alley is to the west.\n"; dest_dir = ({ "room/eastroad4", "north",           "room/eastroad2", "south",           "room/sunalley1", "west" }); }  }
# 9 "room/eastroad3.c"

