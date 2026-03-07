# 1 "room/crop.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/crop.c"
reset(arg) { if (!arg) { set_light( 1); short_desc =  "Fields"; long_desc =  "You are in the middle of the fields where the city grows all its crops.\n"+ "A road runs north of here.\n"; dest_dir = ({ "room/vill_shore", "north" }); }  }
# 7 "room/crop.c"
