# 1 "room/eastroad2.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/eastroad2.c"
reset(arg) { if (!arg) { set_light( 1); short_desc =  "East road"; long_desc =  "East road runs north-south.\n"; dest_dir = ({ "room/eastroad3", "north",           "room/eastroad1", "south" }); }  }
# 7 "room/eastroad2.c"

