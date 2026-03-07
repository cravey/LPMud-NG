# 1 "room/adv_inner2.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/adv_inner2.c"







reset(arg) { if (!arg) { set_light( 1); short_desc =  	 "The LPC board"; long_desc =  	 "This is the LPC discussion room.\n" + 	 "Only wizards can access this room.\n"; dest_dir = ({ "room/adv_inner",  "north" }); } if (!arg) { move_object(clone_object("obj/wiz_bull_board2"), this_object()); } }
# 14 "room/adv_inner2.c"
