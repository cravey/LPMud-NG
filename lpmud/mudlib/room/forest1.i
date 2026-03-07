# 1 "room/forest1.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/forest1.c"



reset(arg) { if (!arg) { set_light( 1); short_desc =  	 "In a forest"; long_desc =  	 "You are in a big forest.\n"; dest_dir = ({ "room/wild1",  "east",  	 "room/clearing",  "west" }); } fix_jacket(); }
# 9 "room/forest1.c"

fix_jacket()
{
    object leather_jacket;

    leather_jacket = present("leather jacket");
    if (!leather_jacket) {
	leather_jacket = clone_object("obj/armour");
	call_other(leather_jacket, "set_name", "leather jacket");
	call_other(leather_jacket, "set_short", "A leather jacket");
	call_other(leather_jacket, "set_alias", "jacket");
	call_other(leather_jacket, "set_long", "A worn but sturdy leather jacket.\n" +
	  "On the back is a lot of rivets making the pattern of a star.\n");
	call_other(leather_jacket, "set_value", 50);
	call_other(leather_jacket, "set_weight", 2);
	call_other(leather_jacket, "set_ac", 2);
	call_other(leather_jacket, "set_type", "armour");
	move_object(leather_jacket, this_object());
    }
}
