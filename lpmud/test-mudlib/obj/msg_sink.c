string seen;

reset(arg) {
    if (arg)
        return;
    seen = "";
    enable_commands();
    set_living_name("msg_sink");
}

short() { return "a message sink"; }
id(str) { return str == "msg_sink" || str == "sink"; }

catch_tell(str) {
    if (!seen)
        seen = "";
    seen += str;
}

clear_seen() {
    seen = "";
    return 1;
}

query_seen() {
    return seen;
}
