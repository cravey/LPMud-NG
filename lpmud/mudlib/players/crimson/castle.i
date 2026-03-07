# 1 "players/crimson/castle.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 465 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "players/crimson/castle.c" 2


# 12 "players/crimson/castle.c"
   

id(str) { return str == "xyzzy"; }

short() {
    write("A narrow prairie footpath leads eastward.\n");
    write("There is a sign here.\n");
}

long() {
    write("Now how the heck did you see this? :-D\n");
}

init() {
    add_action("east"); add_verb("east");
    add_action("south"); add_verb("south");
    add_action("read"); add_verb("read");
    add_action("read"); add_verb("look");
}

read(str) {
    if (str == "sign" || str == "at sign") {
    write("The sign reads: THUNDERDOME is south.\n");
    say(call_other(this_player(), "query_name") + " read a sign.\n");
    return 1;
    } else
    if (str) {
    return 0;
    } else
    if (!str) {
    return 0;
    }
}

east(str) {
    call_other(this_player(), "move_player", "east#players/crimson/mouse");
    return 1;
}

south(str) {
    call_other(this_player(), "move_player", "south#players/crimson/thunderentry");
    return 1;
}

reset(arg) {
    if (arg)
	return;
    move_object(this_object(), "room/plane4");
}

