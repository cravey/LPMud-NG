# 1 "room/adv_inner.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/adv_inner.c"







reset(arg) { if (!arg) { set_light( 1); short_desc =  	 "The inner room of adventurers guild"; long_desc =  "This is the inner room of adventures guild. If you want to discuss LPC,\n" + "then move to the room south from here.\n"+ "Only wizards can access this room.\n"; dest_dir = ({ "room/adv_guild",  "north",  	 "room/adv_inner2",  "south" }); } if (!arg) { move_object(clone_object("obj/wiz_bull_board"), this_object()); } }
# 16 "room/adv_inner.c"
