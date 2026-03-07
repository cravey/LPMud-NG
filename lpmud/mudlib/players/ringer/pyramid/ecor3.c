
short() { return "End of an east/west corridor"; }

long() {
    write(
"This is the end of a stone corridor.  There seem to be two opennings--one\n"+
"to the south the other to the north. The corridor continues west.\n");
    if (call_other("players/ringer/pyramid/nbalance","query_nstate")) {
        write("The openning to the south is blocked by a large stone slab.\n");
        write("The openning to the north is blocked by a large stone slab.\n");
    }
    write("You notice some symbols on the walls.\n");
}

west() {
    call_other(this_player(),"move_player",
               "west#players/ringer/pyramid/ecor2");
    return 1;
}

north() {
    if (call_other("players/ringer/pyramid/nbalance","query_nstate")) {
        write("The passage is blocked!\n");
        return 1;
    }
    call_other(this_player(),"move_player",
               "north#players/ringer/pyramid/nbalance");
    return 1;
}

init() {
    add_action("search"); add_verb("search");
    add_action("north"); add_verb("north");
    add_action("west"); add_verb("west");
    add_action("south"); add_verb("south");
    add_action("look"); add_verb("look");
    add_action("look"); add_verb("l");
    add_action("read"); add_verb("read");
    add_action("read"); add_verb("exa");
}

south() {
    if (call_other("players/ringer/pyramid/nbalance","query_nstate")) {
        write("The passage is blocked!\n");
        return 1;
    }
    call_other(this_player(),"move_player",
               "south#players/ringer/pyramid/sbalance");
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


read(str) {
    if (str != "wall" && str!="writings" && str!="bowls" && str!="walls")
      return 0;
    say(call_other(this_player(),"query_name")
        +" examines the wall.\n");
    if (present("amulet of tongues",this_player()) ||
        present("onyx amulet",this_player())) {
        write("\nThe onyx amulet glows brightly!\n");
        write(
"Intruders: Turn back now before you violate the crypt of Kaunarl, court\n"+
"magician of the great Xyphelod!  Turn back before you incur his great\n"+
              "wrath!\n");
    } else {
        write("There are words but the language is foreign to you.\n");
    }
    write("Below the writing are some symbols:\n");
    write("                       -----          \n");
    write("                      |     |         \n");
    write(" -----    -----       |     |         \n");
    write("|     |  |     |       -----          \n");
    write("|  O  |  |OOOOO|                ----- \n");
    write(" -----    -----                |     |\n");
    write("                               |OOOOO|\n");
    write("                                ----- \n");
    return 1;
}

look(str) {
    if (str == "at slab" || str=="at north slab" || str=="at south slab"){
        write("You see nothing interesting.\n");
        return 1;
    }
    if (str != "at wall" && str != "at writings" && str != "at walls") 
      return 0;
    read("wall");
    return 1;
}
