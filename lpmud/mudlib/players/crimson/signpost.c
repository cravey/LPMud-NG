#define DEST "room/church"

reset(arg) {
    if (arg)
      return;
    move_object(this_object(), "players/crimson/mouse");
}

short() {
    return "A streetsign" ;
}

long() {
    write("    **********************   \n");
    write("    *                    *   \n");
    write("    * South: Mortuary    *\n");
    write("    *                    *    \n");
    write("    * West: THUNDERDOME  *\n");
    write("    *                    *\n");
    write("    **********************    \n");
}

init() {
    add_action("read"); add_verb("read");
}

id(str) {
    return str == "sign" || str == "signpost" || str == "streetsign";
}

read(str) {
    if (!id(str))
	return 0;
    say(call_other(this_player(), "query_name") + " reads a sign.\n");
    long();
    return 1;
}
