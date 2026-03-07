int rstone,bstone;

query_stones() { return rstone*2+bstone; }

short() { return "East/west corridor"; }

long() {
    write(
"This is a short stone corridor. A junction lies to the east.\n");
}

reset() {
    rstone=0;
    bstone=0;
}

init() {
    add_action("search"); add_verb("search");
    add_action("east"); add_verb("east");
    add_action("west"); add_verb("west");
    add_action("move"); add_verb("move");
    add_action("move"); add_verb("push");
    add_action("exam"); add_verb("exa");
    add_action("exam"); add_verb("examine");
    add_action("exam"); add_verb("l");
    add_action("exam"); add_verb("look");
}

exam(str) {
    if (str!="stones" && str!="at stones") return 0;
    l_stones();
    return 1;
}

move(str) {
    if (str != "red stone" && str!="black stone") 
        return 0;
    write("The stone clicks into place.\n");
    if (str=="red stone") {
        if (rstone) 
          rstone = 0;
        else
          rstone = 1;
        return 1;
    }
    if (bstone) 
      bstone = 0;
    else
      bstone = 1;
    return 1;
}

west() {
    call_other(this_player(),"move_player",
               "west#players/ringer/pyramid/tun2");
    return 1;
}

east() {
    call_other(this_player(),"move_player",
               "east#players/ringer/pyramid/sjnct");
    return 1;
}

search(str) {
    if (!str) str = "room";
    if (str == "room") {
        l_stones();
        say(call_other(this_player(),"query_name") + " searches the room.\n");
        return 1;
    }
}

l_stones() {
    write(
          "You notice a two small stones carefully hidden in the floor.\n");
    if (rstone) 
      write(
            "The red stone is in the forward position.\n");
    else
      write(
            "The red stone is in the back position.\n");
    if (bstone) 
      write(
            "The black stone is in the forward position.\n");
    else
      write(
            "The black stone is in the back position.\n");
}
