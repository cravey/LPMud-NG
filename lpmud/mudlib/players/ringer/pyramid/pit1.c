int passage;

reset() {
    passage=0;
}

short() { return "A deep dark pit"; }

long() {
    write(
"This is a deep dark pit.  Those who fall in here are not intended to \n"+
"get out.\n");
    if (passage) 
        write("There is a hole leading north.\n");
}

init() {
    add_action("search"); add_verb("search");
    add_action("north"); add_verb("north");
}

north() {
    if (!passage) return 0;
    call_other(this_player(),"move_player",
               "north#players/ringer/pyramid/stun1");
    return 1;
}

search(str) {
    if (!str) str = "room";
    if (str == "room") {
        say(call_other(this_player(),"query_name") + " searches the room.\n");
        if (random(100) > 70 && !passage) {
            write(
"You notice that one of the stones lining the walls is loose.  With effort\n"+
"you pry it out revealing a passage north.\n");
            passage=1;
        } else 
          write("You find nothing unusual.\n");
        return 1;
    }
}

