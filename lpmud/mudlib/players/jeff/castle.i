# 1 "players/jeff/castle.c"













id(str) { return str == "castle"; }

short() {
    return "Castle of " + "jeff";
}

long() {
    write("This is the " + short() + ".\n");
    write("jeff" + " is a rather new wizard, but it is an amazing castle\n");
    write("just the same. However, the gates are closed.\n");
}

init() {
    add_action("enter", "enter");
}

enter(str) {
    call_other(this_player(), "move_player", "enter#players/jeff/swroom");
    return 1;
}

reset(arg) {
    if (arg)
	return;
    move_object(this_object(), "room/plane8");
}
