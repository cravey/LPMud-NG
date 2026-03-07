# 1 "room/vill_track.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/vill_track.c"




reset(arg) { if (!arg) { set_light( 1); short_desc =  	 "Village track"; long_desc =  "A track going into the village. The track opens up to a road to the east\n" + "and ends with a green lawn to the west.\n"; dest_dir = ({ "room/vill_green", "west",  	 "room/vill_road1", "east" }); } no_castle_flag = 1; }
# 11 "room/vill_track.c"
