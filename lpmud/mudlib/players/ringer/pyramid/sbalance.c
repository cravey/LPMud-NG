short() { return "Moderately sized square room"; }

long() {
    write(
"This is a square, medium sized room.  It is bare and the walls bear no\n"+
"writings or decorations of any kind.\n");
    if (!call_other("players/ringer/pyramid/nbalance","query_nstate")) 
      write("The only obvious exit is to the north.\n");
    else
      write("The only obvious exit is to the south.\n");
}

north() {
    if (call_other("players/ringer/pyramid/nbalance","query_nstate")) 
      return 0;
    call_other(this_player(),"move_player",
               "north#players/ringer/pyramid/ecor3");
    return 1;
}

init() {
    add_action("search"); add_verb("search");
    add_action("north"); add_verb("north");
    add_action("south"); add_verb("south");
}

south() {
    if (!call_other("players/ringer/pyramid/nbalance","query_nstate")) 
      return 0;
    call_other(this_player(),"move_player",
               "south#players/ringer/pyramid/mgcrypt");
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

carry() {
    return first_inventory(this_object());
}
