# 1 "room/sunalley1.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/sunalley1.c"
reset(arg) { if (!arg) { set_light( 1); short_desc =  "Sun alley"; long_desc =  "Sun alley runs east-west.\n"; dest_dir = ({ "room/sunalley2", "west",           "room/eastroad3", "east" }); }  }
# 7 "room/sunalley1.c"

