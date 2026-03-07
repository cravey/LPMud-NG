# 1 "room/orc_treasure.c"
# 1 "room/room.h"
inherit "room/room";
























# 2 "room/orc_treasure.c"

object gold_stick, orc_slayer, shaman;




fix_shaman()
{
    if (!shaman || !living(shaman)) {
	gold_stick = clone_object("obj/treasure");
	call_other(gold_stick, "set_id", "staff");
	call_other(gold_stick, "set_alias", "golden staff");
	call_other(gold_stick, "set_short", "A golden staff");
	call_other(gold_stick, "set_value", 300);
	orc_slayer = clone_object("obj/weapon");
	call_other(orc_slayer, "set_name", "short sword");
	call_other(orc_slayer, "set_alias", "sword");
	call_other(orc_slayer, "set_short", "Short sword");
	call_other(orc_slayer, "set_alt_name", "orc slayer");
	call_other(orc_slayer, "set_long", "This is a very fine blade.\n"+
		"It's covered with ancient runes.\n" + 
		"Engraved on it is a picture of the sword slicing an orc.\n");
	call_other(orc_slayer, "set_read", 
		"The only thing you can read is the word 'orc'.\n");
	call_other(orc_slayer, "set_class", 9);
	call_other(orc_slayer, "set_weight", 2);
	call_other(orc_slayer, "set_value", 200);
	call_other(orc_slayer, "set_hit_func", this_object());
	shaman = clone_object("obj/monster.talk");
	call_other(shaman, "set_name", "shaman");
	call_other(shaman, "set_alias", "orc shaman");
	call_other(shaman, "set_race", "orc");
	call_other(shaman, "set_level", 10);
	call_other(shaman, "set_al", -300);
	call_other(shaman, "set_short", "An orc shaman");
	call_other(shaman, "set_wc", 10);
	call_other(shaman, "set_ac", 1);
	call_other(shaman, "set_aggressive", 1);
	call_other(shaman, "set_chance", 20);
	call_other(shaman, "set_spell_mess1",
	    "You are hit by a magic missile");
	call_other(shaman, "set_spell_mess2",
	    "The shaman casts an magic missile");
	call_other(shaman, "set_spell_dam", 20);
	move_object(shaman, this_object());
	move_object(gold_stick, shaman);
	move_object(orc_slayer, shaman);
    }
}

reset(arg) { if (!arg) { set_light( 1); short_desc =  	 "The orc treasury"; long_desc =  	 "You are in the orc treasury. It is normally heavily guarded.\n"; dest_dir = ({ "room/fortress",  "south" }); } fix_shaman(); }
# 55 "room/orc_treasure.c"

weapon_hit(attacker)
{
    string alig;

    
    if(call_other(attacker,"id","orc")){
	write("Ziiing\n");
	return 10;
    }
    return 0;
}
