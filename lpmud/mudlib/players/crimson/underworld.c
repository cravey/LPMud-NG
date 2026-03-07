string rope;
reset(arg)
{
    if (!arg)
	set_light(0);
}

init()
{
    add_action("up"); add_verb("up");
    add_action("up"); add_verb("climb");
    add_action("down"); add_verb("down");
    add_action("down"); add_verb("climb");
    add_action("swing"); add_verb("swing");
}

up()
{
    call_other(this_player(), "move_player", "up the rope#players/crimson/greeting");
    return 1;
}

swing() {
     tell_object(this_player(), "Whoa, whoa, WHOA!\n");
     tell_object(this_player(), "You're not going to make it.\n");
     tell_object(this_player(), "You're not going to make it!\n");
     call_other(this_player(), "move_player", "swinging wildly on the rope#players/crimson/underworld3");
     return 1;
}

long()
{
    write("After climbing down the rope about fifteen feet you start to\n");
    write("suffer from vertigo. The floor of this cavern is still not in sight.\n");
    write("However, you see large stalactites hanging from the ceiling and to\n");
    write("the east you can see a ledge and a dark opening.  \n");
}

short() {
    return "Under the greeting hall";
}

down() {
    call_other(this_player(), "move_player", "down the rope#players/crimson/underworld4");
    return 1;
}

realm() { return "NT"; }
