reset(arg)
{
}

init()
{
    add_action("south"); add_verb("south");
    add_action("north"); add_verb("north");
    add_action("east"); add_verb("east");
}

north()
{
    if (call_other("players/ringer/trap3","query_door")) {
        call_other(this_player(), "move_player", "north#players/ringer/trap3");
    } else {
        write("The door to the north is closed.\n");
    }
    return 1;
}

south()
{
    call_other(this_player(),"move_player", "south#players/ringer/trap1");
    return 1;
}

east()
{
    call_other(this_player(),"move_player", "east#players/ringer/tun3");
    return 1;
}

long()
{
    write("This is the north end of a long hall. A passage extends east\n");
    write("and south. There is an entrance to a small room to the north.\n");
}

short() {
    return "The end of a long hall";
}

