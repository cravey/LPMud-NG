int nstate;
object sb,ec,tb,mc;

query_nstate() { return nstate;}

reset() {
    object ob;
    nstate=0;
    call_other("players/ringer/pyramid/sbalance","reset");
    sb = find_object("players/ringer/pyramid/sbalance");
    call_other("players/ringer/pyramid/ecor3","reset");
    ec=find_object("players/ringer/pyramid/ecor3");
    call_other("players/ringer/pyramid/topbalance","reset");
    tb=find_object("players/ringer/pyramid/topbalance");
    call_other("players/ringer/pyramid/mgcrypt","reset");
    mc=find_object("players/ringer/pyramid/mgcrypt");
    set_heart_beat(1);
    if (!present("ringer_load_stone")) {
        ob = clone_object("players/ringer/pyramid/bgstone");
        move_object(ob,this_object());
    }
}

short() { return "Moderately sized square room"; }

long() {
    write(
"This is a square, medium sized room.  It is bare and the walls bear no\n"+
"writings or decorations of any kind.\n");
    if (nstate) 
      write("The only obvious exit is to the north.\n");
    else
      write("The only obvious exit is to the south.\n");
}

north() {
    if (!nstate)
      return 0;
    call_other(this_player(),"move_player",
               "north#players/ringer/pyramid/topbalance");
    return 1;
}

init() {
    add_action("search"); add_verb("search");
    add_action("north"); add_verb("north");
    add_action("south"); add_verb("south");
}

south() {
    if (nstate) 
      return 0;
    call_other(this_player(),"move_player",
               "south#players/ringer/pyramid/ecor3");
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


heart_beat() {
    if (nstate && first_inventory(this_object())) {
        tell_room(this_object(),
"You hear the sound of sliding stone and the room shifts.  Gradually the\n"+
"room descends.  The exit to the north is blocked off as the room settles\n"+
"down into its shaft, and a new exit appears to the south.\n");
        nstate=0;
        tell_room(tb,
"A stone slab descends blocking the door to the south.\n");
        tell_room(mc,
"A stone slab lifts blocking the door to the north.\n");
        tell_room(sb,
"You hear the sound of sliding stone and the room shifts.  Gradually the\n"+
"room ascends.  The exit to the south is blocked off as the room rises\n"+
"up into its shaft, and a new exit appears to the north.\n");
        tell_room(ec,
"To the north a slab of stone slides down to reveal an exit north.  At the\n"+
"same time a slab to the south slides up to reveal another exit.\n");
    }
    if (!nstate && !first_inventory(this_object()) 
        && call_other("players/ringer/pyramid/sbalance","carry")) {
        nstate=1;
        tell_room(sb,
"You hear the sound of sliding stone and the room shifts.  Gradually the\n"+
"room descends.  The exit to the north is blocked off as the room settles\n"+
"down into its shaft, and a new exit appears to the south.\n");
        tell_room(ec,
"To the north a slab of stone slides up, blocking an exit north.  At the\n"+
"same time a slab to the south slides down blocking that exit also.\n");
        tell_room(tb,
"A stone slab lifts openning the door to the south.\n");
        tell_room(mc,
"A stone slab descends openning the door to the north.\n");
    }
}
        
