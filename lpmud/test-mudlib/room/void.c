reset(arg) {
    if (arg)
        return;
    set_light(1);
}

short() { return "The void"; }

long() {
    write("You are in an empty bootstrap void.\n");
}

id(str) { return str == "void"; }
