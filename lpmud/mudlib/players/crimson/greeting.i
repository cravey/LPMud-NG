# 1 "players/crimson/greeting.c"
string rope;
reset(arg)
{
    if (!arg)
	set_light(1);
    extra_reset();
}

extra_reset()
{
    object panther, leash;
    int i;

    i = 0;
    if (!present("panther")) {
	while(i<2) {
	    i += 1;
	    panther = clone_object("obj/monster");
	    call_other(panther, "set_name", "panther");
            call_other(panther, "set_short", "A grey panther");
	    call_other(panther, "set_alias", "grey panther");
	    call_other(panther, "set_race", "feline");
            call_other(panther, "set_level", 5);
	    call_other(panther, "set_hp", 90);
	    call_other(panther, "set_ep", 8000);
	    call_other(panther, "set_al", -100);
	    call_other(panther, "set_ac", 3);
            call_other(panther, "set_wc", 13);
	    call_other(panther, "set_aggressive", 1);
            move_object(panther, this_object());
            leash = clone_object("obj/treasure");
            call_other(leash, "set_id", "leash");
            call_other(leash, "set_short", "A grayish leash");
            call_other(leash, "set_long", "The leash is still warm from the feline's body.\n");
            call_other(leash, "set_weight", 1);
            call_other(leash, "set_value", 250);
            transfer(leash, panther);
	}
    }
}
init()
{
    add_action("north"); add_verb("north");
    add_action("west"); add_verb("west");
    add_action("east"); add_verb("east");
    add_action("up"); add_verb("up");
    add_action("enter"); add_verb("enter");
    add_action("enter"); add_verb("climb");
    add_action("search"); add_verb("search");
    add_action("look"); add_verb("look");
}

search(str)
{
    if (!id(str)) {
        write("What do you want to search?\n");
	return 1;
    } else
    if (str == "vat") {
    write("Staring down into the HUGE vat you see something sparkling in the light.\n");
    write("It looks like it might be some sort of liquid but, its too far down to tell.\n");
    return 1;
    } else
    if (str == "fountain") {
    write("The cherub is life-like.  Hmmm, it looks like there there are some strands of something here.\n");
    return 1;
    }
}

look(str) {
   if (str == "at cherub") {
   write("The cherub smiles down at you.\n");
   return 1;
   } else
   if (!str) {
   return 0;
   } else
   if (str == "at vat") {
   write("Staring down into the HUGE vat you see something sparkling in the light.\n");
   write("It looks like it might be some sort of liquid but it is too far down to tell.\n");
   return 1;
   } else
   if (str == "at fountain") {
   write("Hmm, it looks like there are some strands of something on it.\n");
   return 1;
   }
}

up() {
    call_other(this_player(), "move_player", "up the stairs#players/crimson/landing");
    return 1;
}

north()
{
    call_other(this_player(), "move_player", "north#players/crimson/entry");
    return 1;
}

west() {
    call_other(this_player(), "move_player", "west#players/crimson/archw1");
    return 1;
}

east() {
    call_other(this_player(), "move_player", "east#players/crimson/vampi");
    return 1;
}
long()
{
    write("This is a huge entryway.  The mortician's probably use it as a \n");
    write("waiting area for the funeral parlor's mourners.  A small fountain\n");
    write("bubbles in the center of the room.  In the center of the fountain\n");
    write("is the statue of a small cherub. He spurts cold water into\n");
    write("a shiney silver vat.  Multi-colored tiles cover the floor. To the\n");
    write("east and west are grand oak archways.  A red carpet covers the staircase\n");
    write("which leads up to a small landing.  Fine and detailed paintings cover\n");
    write("the domed ceiling which looms overhead.  North, the door out stands\n");
     write("wide open. It is very tempting to leave this place.\n");
}

short() {
    return "The greeting hall";
}

enter(str) {
    if (!rope) {
        write("It is pretty far down. You had better find a way to support yourself.\n");
	return 1;
    } else
    if (str != "vat") {
    write("What do you want to enter?\n");
    return 1;
    } else
    call_other(this_player(), "move_player", "down through the vat#players/crimson/underworld");
    return 1;
}

tie(str)
{
    if (str != "fountain" && str != "vat")
        return 0;
    rope = 1;
    return 1;
}

id(str) {
    return str == "fountain" || str == "vat";
}

untie(str) {
    rope = 0;
    return 1;
}

query_rope() {
    return rope;
}
