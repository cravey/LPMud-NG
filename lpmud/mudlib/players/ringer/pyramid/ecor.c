
short() { return "End of an east/west hallway"; }

long() {
    write(
"This is the end of a short stone hallway.  There is a small room to the\n"+
"east and a larger one to the north.  The hallway continues west.\n");
    if (!call_other("players/ringer/pyramid/wtempgate","query_esdoor")) 
        write("The door to the north is closed.\n");
}

east() {
    call_other(this_player(),"move_player",
               "east#players/ringer/pyramid/esmroom");
    return 1;
}

west() {
    call_other(this_player(),"move_player",
         "west#players/ringer/pyramid/pyr4");
    return 1;
}

init() {
    add_action("search"); add_verb("search");
    add_action("north"); add_verb("north");
    add_action("east"); add_verb("east");
    add_action("west"); add_verb("west");
}

north() {
    if (!call_other("players/ringer/pyramid/wtempgate","query_esdoor")) {
        write("The door to the north is closed.\n");
        return 1;
    }
    call_other(this_player(),"move_player",
               "north#players/ringer/pyramid/etempgate");
    return 1;
}

search(str) {
    if (!str) str = "room";
    if (str == "room") {
        write(
"You notice that the doorway of the room to the north is grooved.\n");
        say(call_other(this_player(),"query_name") + " searches the room.\n");
        return 1;
    }
}

