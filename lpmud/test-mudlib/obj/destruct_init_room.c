reset(arg) {
    if (arg)
        return;
}

short() { return "destruct-on-init room probe"; }
id(str) { return str == "destruct_init_room" || str == "probe_room"; }

init() {
    destruct(this_object());
}
