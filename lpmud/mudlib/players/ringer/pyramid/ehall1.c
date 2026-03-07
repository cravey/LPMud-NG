int stone;

query_stone() { return stone; }

short() { return "End of an east/west hallway"; }

long() {
    write(
"This is the end of a short stone hallway. A large room lies to the west.\n");
}

reset() {
    stone=0;
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
    if (str!="stone" && str!="at stone") return 0;
    l_stones();
    return 1;
}

move(str) {
    if (str != "stone") 
        return 0;
    say(call_other(this_player(),"query_name")+" moves the stone.\n");
    write("The stone clicks into place.\n");
    if (stone) 
      stone = 0;
    else
      stone = 1;
    return 1;
}

east() {
    call_other(this_player(),"move_player",
               "east#players/ringer/pyramid/ehall2");
    return 1;
}

west() {
    call_other(this_player(),"move_player",
               "west#players/ringer/pyramid/temp");
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
          "You notice a small stone carefully hidden in the floor.\n");
    if (stone) 
      write(
            "The stone is in the forward position.\n");
    else
      write(
            "The stone is in the back position.\n");
}


