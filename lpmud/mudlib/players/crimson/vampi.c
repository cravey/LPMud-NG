string lid;
object vampire;
int lid_open;
reset(arg) {
    if (vampire && living(vampire)) {
    lid_open = 1;
    return 1;
    } else
    if (!vampire || !living(vampire)) {
    lid_open = 0;
    return 1;
    }
    if (!arg)
	set_light(1);
    open();
}

open(str) {
    object vampire, armor;
    string lid, coffin;
    if (!id(str))
       return 1;
    else {
      if (lid_open == 1) {
      say(call_other(this_player(), "query_name") + " stares into the coffin.\n");
      write("SSSSSSSqqqqqqqqquuuuuueeeeeaaaakkkkkkk\n");
      return 1;
    } else {
      lid_open = 1;
      say(call_other(this_player(), "query_name") + " opens the lid of the coffin.\n");
      say("SSSSSSSSqqqqqqqqquuuuuueeeeeaaaakkkkkkk\n");
      say("The is a huge red vaporous gas forming!\n");
      say("The vampire stares at you hungryly.\n");
      write("SSSSSSSqqqqqqqquuuuuuuuueeeeeeaaaaaaakkkkkkk\n");
      write("A red vaporous gas is solidifying before you!\n");
      write("The vampire eyes you hungryly.\n");

    if (id(str))

    if (!vampire || !living(vampire)) {
    object player;
            player = this_player();
	    vampire = clone_object("obj/monster");
	    call_other(vampire, "set_name", "vampire");
	    call_other(vampire, "set_alias", "count");
	    call_other(vampire, "set_race", "undead");
            call_other(vampire, "set_level", 17);
	    call_other(vampire, "set_hp", 200);
            call_other(vampire, "set_wc", 30);
	    call_other(vampire, "set_al", -800);
            call_other(vampire, "set_short", "Count Vampisray the vampire (REAL NASTY!)");
	    call_other(vampire, "set_ac", 9);
            call_other(vampire, "set_eps", 1000000);
            call_other(vampire, "set_chance", 40);
            call_other(vampire, "set_spell_mess1", "The vampire hisses.\n");
            call_other(vampire, "set_spell_mess2", "The vampire gazes into" +
              " your eyes. You whince in pain.\n");
            call_other(vampire, "set_spell_dam", 20);
	    call_other(vampire, "set_aggressive", 1);
            move_object(vampire, this_object());
            call_other(vampire, "attack_object", player);
            armor = clone_object("obj/armor");
            call_other(armor, "set_name", "gauntlets");
            call_other(armor, "set_short", "A pair of gauntlets");
            call_other(armor, "set_long", "Hmmm, these gauntlets have strange runes inscribed on them.\n");
            call_other(armor, "set_ac", 1);
            call_other(armor, "set_weight", 1);
            call_other(armor, "set_type", "gloves");
            call_other(armor, "set_value", 1000);
            transfer(armor, vampire);
         }
       }
    }
}
init()
{
    add_action("west"); add_verb("west");
    add_action("open"); add_verb("open");
    add_action("look"); add_verb("look");
}

west()
{
    call_other(this_player(), "move_player", "west#players/crimson/greeting");
    return 1;
}

look(str) {
   if(str == "at coffin") {
   write("A shiney black coffin.\n");
   return 1;
   } else
   if(str == "at lid") {
   write("The lid looks like it is partially open.\n");
   return 1;
   
    }
}

id(str) {
    return str == "lid" || str == "coffin";
}

long()
{
    write("It seems darker here.  In the shadows you can make out the shape\n");
    write("of a skeleton on the floor. It looks as though this room hasn't\n");
    write("been touched in years. On a pedastal in the center of the room\n");
    write("you see a coffin with the lid partially open.  The air here smells\n");
    write("stale, like the smell of rotting flesh.\n");
    write("You see the fountain through the archway to the west.\n");
}

short() {
    return "The coffins";
}
