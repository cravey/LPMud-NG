
short() { return "A small room"; }

long() {
    write(
"This is a small hard to find room.  The walls here are hard and dark.\n"+
"A small hole leads down and a doorway leads south.\n");
    if (!call_other("players/ringer/pyramid/nbalance","query_nstate")) {
        write("The openning to the south is blocked by a large stone slab.\n");
    }
}

down() {
    call_other(this_player(),"move_player",
               "down#players/ringer/pyramid/nmaze10");
    return 1;
}

init() {
    add_action("search"); add_verb("search");
    add_action("down"); add_verb("down");
    add_action("south"); add_verb("south");
    add_action("exam"); add_verb("look");
    add_action("exam"); add_verb("exa");
    add_action("exam"); add_verb("examine");
    add_action("exam"); add_verb("l");
}

exam(str) {
    if (str=="at slab" || str=="slab") {
        write("There is nothing special about the slab.\n");
        return 1;
    }
}
 
south() {
    if (!call_other("players/ringer/pyramid/nbalance","query_nstate")) {
        write("The passage is blocked!\n");
        return 1;
    }
    call_other(this_player(),"move_player",
               "south#players/ringer/pyramid/nbalance");
    return 1;
}

search(str) {
    if (!str) str = "room";
    if (str == "room") {
        write("You notice nothing unusual.\n");
        say(call_other(this_player(),"query_name") + " searches the room.\n");
        return 1;
    }
}
