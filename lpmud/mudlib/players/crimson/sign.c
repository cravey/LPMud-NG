reset(arg) {
    if (arg)
        return;
    move_object(this_object(), "room/church");
}

short() {
    return "A treasure map" ;
}

long() {
    write("######################################\n");
    write("#                                    #\n");
    write("# Hiya, You weren't in so here's a   
    write("#         #                          #\n");
    write("#         # # #                      #\n");
    write("#         #   #                      #\n");
    write("#         #   #                      #\n");
    write("#         # # X (stream) #######(sea)#\n");
    write("#                                    #\n");
    write("######################################\n");
}

init() {
    add_action("read"); add_verb("read");
}

id(str) {
    return str == "map" || str == "treasure map" || str == "treasuremap";
}

read(str) {
    if (!id(str))
	return 0;
    say(call_other(this_player(), "query_name") + " reads a treasure map.\n");
    long();
    return 1;
}

query_weight() { return 1; }

get() { return 1; }

query_value() { return 5; }
