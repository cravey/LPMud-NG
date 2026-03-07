# 1 "room/forest2.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/forest2.c"

object troll;





extra_reset() {
    object money;
    if (!troll || !living(troll)) {
	troll = clone_object("obj/monster");
	call_other(troll, "set_name", "troll");
	call_other(troll, "set_level", 9);
	call_other(troll, "set_hp", 100);
	call_other(troll, "set_wc", 12);
	call_other(troll, "set_al", -60);
	call_other(troll, "set_short", "A troll");
	call_other(troll, "set_long",
		   "It is a nasty troll that look very aggressive.\n");
	call_other(troll, "set_aggressive", 1);
	call_other(troll, "set_spell_mess1",
		   "The troll says: Mumble");
	call_other(troll, "set_spell_mess2",
		   "The troll says: Your mother was a bitch!");
	call_other(troll, "set_chance", 20);
	move_object(troll, this_object());
	money = clone_object("obj/money");
        call_other(money, "set_money", random(500));
        move_object(money, troll);
    }
}

reset(arg) { if (!arg) { set_light( 1); short_desc =          "In a forest"; long_desc =          "You are in a big forest.\n"; dest_dir = ({ "room/clearing",  "east",  	 "room/slope",  "west" }); } extra_reset(); }
# 39 "room/forest2.c"
