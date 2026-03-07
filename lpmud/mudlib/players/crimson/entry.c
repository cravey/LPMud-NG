string bell, doorbell;
object lurch;
int door_open;
reset(arg) {
    set_light(1);
    if(!find_living("lurch")) 
    ring();
}

ring(str) {
    object lurch, weapon;
    string bell, doorbell;
    if (!id(str))
       return 1;
    else {
      if (door_open == 1) {
      say(call_other(this_player(), "query_name") + " leans on the doorbell.\n");
      write("Ring, ring..... ring, ring.....\n");
      return 1;
    } else {
      door_open = 1;
      say(call_other(this_player(), "query_name") + " leans on the doorbell.\n");
      say("Ring, ring .... ring, ring ....\n");
      say("The door squeaks loudly as it opens.\n");
      say("Lurch arrives.\n");
      write("Ring, ring, ...... ring, ring ......\n");
      write("The door squeaks loudly as it opens.\n");
      write("Lurch arrives.\n");
      write("Lurch says: You rang, sir?\n");

    if (id(str))

    if (!lurch || !living(lurch)) {
	    lurch = clone_object("obj/monster");
	    call_other(lurch, "set_name", "lurch");
	    call_other(lurch, "set_short", "Lurch the Undertaker");
	    call_other(lurch, "set_alias", "undertaker");
	    call_other(lurch, "set_race", "thing");
            call_other(lurch, "set_level", 11);
            call_other(lurch, "set_wc", 11);
	    call_other(lurch, "set_hp", 120);
	    call_other(lurch, "set_al", 200);
	    call_other(lurch, "set_ac", 4);
	    call_other(lurch, "set_aggressive", 0);
            weapon = clone_object("obj/weapon");
            call_other(weapon, "set_name", "net");
            call_other(weapon, "set_alt_name", "butterfly net");
            call_other(weapon, "set_short", "A butterfly net");
            call_other(weapon, "set_long", "A nylon butterfly net.\n");
            call_other(weapon, "set_class", 12);
            call_other(weapon, "set_weight", 2);
            call_other(weapon, "set_value", 200);
            transfer(weapon, lurch);
            move_object(lurch, this_object());
         }
       }
    }
}
init()
{
    add_action("south"); add_verb("south");
    add_action("north"); add_verb("north");
    add_action("ring"); add_verb("ring");
}

south()
{
    if (present("lurch")) {
        write("Lurch refuses to let you go through the doorway.\n");
	return 1;
    } else if (door_open == 0) {
      write("The doors are locked.\n");
    } else {
    call_other(this_player(), "move_player", "south#players/crimson/greeting");
    }
    return 1;
}

north()
{
    call_other(this_player(), "move_player", "north#players/crimson/mouse");
    return 1;
}

id(str) {
    return str == "bell" || str == "doorbell";
}

long()
{
    write("Dark clouds loom overhead. A few sparse pine trees surround\n");
    write("a huge old house. The smell of disinfectant lingers in the air.\n");
    write("Before you is the town mortuary. Steep rickety steps\n");
    write("give way under your feet. Two heavey oak doors are the\n");
    write("only means to enter. A sign next to the doors reads:\n");
    write("For service ring bell.\n");
}

short() {
    return "Funeral Parlor";
}
