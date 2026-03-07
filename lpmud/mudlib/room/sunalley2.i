# 1 "room/sunalley2.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/sunalley2.c"
reset(arg) { if (!arg) { set_light( 1); short_desc =  "Sun alley"; long_desc =  "Sun alley runs east from here.\n"; dest_dir = ({ "room/sunalley1", "east" }); }  }
# 6 "room/sunalley2.c"
