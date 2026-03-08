reset(arg) {
    if (arg)
        return;
    set_light(1);
}

short() { return "Engine Test Lab"; }

long(str) {
    if (str == "console") {
        write("A diagnostic console hums quietly.\n");
        return;
    }
    write("You are in the engine test lab.\n");
    write("A console is bolted to the floor.\n");
    write("The only obvious exit is west.\n");
}

id(str) {
    return str == "console";
}

init() {
    add_action("go_west", "west");
}

go_west() {
    call_other(this_player(), "move_player", "west#room/church");
    return 1;
}
