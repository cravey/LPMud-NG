# 1 "room/eastroad5.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/eastroad5.c"
reset(arg) { if (!arg) { set_light( 1); short_desc =  "East road"; long_desc =  "East road runs south from here.\n"+ "To the west lies the Eastroad Inn.\n"; dest_dir = ({ "room/eastroad4", "south",  "room/inn", "west" }); }  }
# 8 "room/eastroad5.c"
