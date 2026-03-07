# 1 "room/vill_shore2.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/vill_shore2.c"
reset(arg) { if (!arg) { set_light( 1); short_desc =  	 "Village shore"; long_desc =  "The village shore. A jetty leads out to the east. To the north some stairs\n"+ "leads down to the north beach. A road starts to the west\n"; dest_dir = ({ "room/jetty", "west",  	 "room/jetty2", "east" }); }  }
# 7 "room/vill_shore2.c"
