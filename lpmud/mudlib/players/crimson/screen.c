int current_page;
short() {
    return "A TV screen" ;
}

reset(arg) {
    current_page = 1;
    move_object(this_object(), "room/church");
}

long() {
    if (current_page == 1) {
    cat("/NEWSPAPER");
    current_page = current_page + 1;
    say(call_other(this_player(), "query_name") + " looks at the TV screen.\n");
    } else
    if (current_page == 2) {
    cat("/THUNDERMAP");
    current_page = 1;
    say(call_other(this_player(), "query_name") + " looks at the TV screen.\n");
    return 1;
    }
}

init() {
    add_action("read"); add_verb("read");
    add_action("watch"); add_verb("watch");
}

id(str) {
    return str == "tv screen" || str == "TV screen" || str == "screen" || str == "tv" || str == "television";
}

read(str) {
    if (str == "TV screen" && (current_page == 1) || str == "screen" && (current_page == 1)) {
    cat("/NEWSPAPER");
    current_page = current_page + 1;
    say(call_other(this_player(), "query_name") + " reads the screen.\n");
    return 1;
    } else
    if (str == "TV screen" && (current_page == 2) || str == "screen" && (current_page == 2)) {
    cat("/THUNDERMAP");
    current_page = 1;
    say(call_other(this_player(), "query_name") + " reads the screen.\n");
    return 1;
    }
}

get() { return 0; }

query_value() { return 5; }

watch() {
   long();
   return 1;
}
