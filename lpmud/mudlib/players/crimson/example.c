short() { return "Example Hotel Room"; }

reset(arg) {
     set_light(1);
}

long() {
     write("This is a sample hotel room.\n");
     write("The only obvious exit is west.\n");
     return 1;
}

init() {
    add_action("west"); add_verb("west");
}

west(str) {
    call_other("players/crimson/hall", "set_who", 0);
    call_other(this_player(), "move_player", "west#players/crimson/hall");
    call_other("players/crimson/hall", "set_there", 0);
    return 1;
}
