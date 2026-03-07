
short() { return "Crypt of Xyphelod"; }

long() {
    write(
"This is the REAL Crypt of King Xyphelod. The walls are decorated lavishly.\n"+
"The entire family was buried here and a passage to the south probably\n"+
"leads to the royal treasure room.\n");
}

reset() {
    set_light(1);
    if (!present("crypt demon")) {
        make_demon();
        make_demon();
    }
}

south() {
    if (present("crypt demon")) {
        write("The crypt demon prevents you!\n");
        return 1;
    }
    call_other(this_player(),"move_player",
               "south#players/ringer/pyramid/xytr");
    return 1;
}

init() {
    object ob;
    if (!present("mute_curse",this_player()) 
        && !call_other(this_player(),"query_npc")) {
        write(
"A voice booms from nowhere:\n"+
"You have found the crypt of Xyphelod, but you shall never tell a soul!\n"+
"Behold the curse of Xyphelod!\n");
        ob = clone_object("players/ringer/pyramid/tcurse");
        call_other(ob,"start",this_player());
    }
    add_action("search"); add_verb("search");
    add_action("south"); add_verb("south");
}

search(str) {
    if (!str) str = "room";
    if (str == "room") {
        write("You notice nothing unusual.\n");
        say(call_other(this_player(),"query_name") + " searches the room.\n");
        return 1;
    }
}

object demon;

make_demon() {
    int t;
    
    t = random(5);
    demon = clone_object("obj/monster");
    call_other(demon,"set_name","crypt demon");
    call_other(demon,"set_alias","demon");
    call_other(demon,"set_short","A crypt demon");
    call_other(demon,"set_long",
"The demon must have been summoned by Kaunarl to protect the treasure of\n"+
"Xyphelod!\n");
    call_other(demon,"set_level",10+t);
    call_other(demon,"set_ac",3+t);
    call_other(demon,"set_wc",8+t);
    call_other(demon,"set_aggressive",1);
    call_other(demon,"set_chance",30);
    call_other(demon,"set_spell_mess1",
"The demon's eyes flash brightly!");
    call_other(demon,"set_spell_mess2","You are hit with a searing flash!");
    call_other(demon,"set_spell_dam",25);
    call_other(demon,"set_align",-500);
    move_object(demon,this_object());
}
