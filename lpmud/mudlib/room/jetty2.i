# 1 "room/jetty2.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/jetty2.c"









reset(arg) { if (!arg) { set_light( 1); short_desc =  	 "Jetty"; long_desc =  "You are at a jetty. The waves rolls in from east.\nA small path leads back to west.\n"; dest_dir = ({ "room/vill_shore2",  "west",  	 "room/sea",  "east" }); } no_castle_flag=1; if (!present("bag")) move_object(clone_object("obj/bag"), this_object()); }
# 16 "room/jetty2.c"
