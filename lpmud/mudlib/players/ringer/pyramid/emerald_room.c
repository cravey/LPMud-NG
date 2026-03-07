object emerald;

reset() {
    set_light(1);
    if (!present("ringer_emerald")) {
        emerald = clone_object("players/ringer/mitem");
        call_other(emerald,"set_id","emerald");
        call_other(emerald,"set_alias","ringer_emerald");
        call_other(emerald,"set_short","A small five sided emerald");
        call_other(emerald,"set_long",
"An exquisite five sided emerald. The gem cutter who made it was a master\n"+
                   "of his craft.\n");
        call_other(emerald,"set_value",200);
        call_other(emerald,"set_weight",2);
        move_object(emerald,this_object());
    }
}

short() { return "Brilliant emerald green room"; }

long() {
    write(
"This room is completely filled with green light.  In the center of the\n"+
"room is a pedestal for a gem of some sort.  There is a small hole leading\n"+
"north.\n");
}

init() {
    add_action("search"); add_verb("search");
    add_action("north"); add_verb("north");
    add_action("put"); add_verb("put");
    add_action("put"); add_verb("insert");
    add_action("exam"); add_verb("exa");
    add_action("exam"); add_verb("examine");
    add_action("exam"); add_verb("l");
    add_action("exam"); add_verb("look");
}

exam(str) {
    if (str == "pedestal" || str == "at pedestal") {
        write(
"The pedestal is simple, green and elegant.  It seems to be an integral\n"+
"part of the room.  There is no seem in its junction with the floor.\n");
        if (present("ringer_emerald",this_object())) 
          write("Atop the pedestal is a beautiful five sided emerald!\n");
        return 1;
    }
    if (str!="hole" && str!="small hole" && str !="at hole" 
        && str!="at small hole") return 0;
    write(
"The hole is small and deep.  If you dropped anything into it you would\n"+
"probably not be able to retrieve it.  The sides of the hole are smooth\n"+
"and polished as though things were meant to slide easily down the shaft.\n");
    return 1;
}


put(str) {
    string what,where;
    object ob;
    if (sscanf(str,"%s in %s",what,where)<2) return 0;
    if (what == "all") {
        write("Put things in one at a time!  That is a small hole!\n");
        return 1;
    }
    ob = present(what,this_player());
    if (!ob) {
        write("Put what?\n");
        return 1;
    }
    if (where != "hole" && where!="small hole") 
      return 0;
    say(call_other(this_player(),"query_name")
        + " puts "+lower_case(call_other(ob,"short"))+" into the hole.\n");
    write(
          "It slides into the hole out of sight.\n");
    say(
        "It slides into the hole out of sight.\n");
    move_object(ob,"players/ringer/pyramid/pit1");
    return 1;
}

north() {
    if (carry_any(this_player())) {
        write("You are carrying too much.\n");
        return 1;
    }
    call_other(this_player(),"move_player",
               "north#players/ringer/pyramid/emerald_entry");
    return 1;
}

carry_any(ob) {
    object in;

    in = first_inventory(ob);
    while (in && call_other(in,"id","ND")) 
      in = next_inventory(in);
    return in;
}

search(str) {
    if (!str) str = "room";
    if (str == "room") {
        say(call_other(this_player(),"query_name") + " searches the room.\n");
        write("You find a small hole in the floor.\n");
        return 1;
    }
}

