reset(arg) {
    if (arg)
        return;
    set_light(1);
}

short() { return "Engine Test Church"; }

long(str) {
    if (str == "sign") {
        write("Sign: east leads to the test lab.\n");
        return;
    }
    write("You are in the engine test church.\n");
    write("A sign is mounted on the wall.\n");
    write("The only obvious exit is east.\n");
}

id(str) {
    return str == "sign";
}

init() {
    add_action("go_east", "east");
}

go_east() {
    call_other(this_player(), "move_player", "east#room/lab");
    return 1;
}
