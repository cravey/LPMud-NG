# 1 "room/deep_forest1.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/deep_forest1.c"

reset(arg) { if (!arg) { set_light( 1); short_desc =  	 "Deep forest"; long_desc =  	 "In the deep forest. The wood lights up to the east.\n"; dest_dir = ({ "room/plane12",  "east" }); }  }
# 6 "room/deep_forest1.c"
