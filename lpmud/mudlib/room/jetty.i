# 1 "room/jetty.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/jetty.c"



reset(arg) { if (!arg) { set_light( 1); short_desc =           "Road"; long_desc =  "You are on a road going out of the village. To the east the road widens out\n"+ "as it leads down to the shore. To the west lies the city.\n"; dest_dir = ({ "room/vill_shore", "west",           "room/vill_shore2", "east" }); } no_castle_flag=1; }
# 11 "room/jetty.c"
