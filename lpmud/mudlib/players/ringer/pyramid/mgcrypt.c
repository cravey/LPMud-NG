
short() { return "Crypt of Kaunarl"; }

int passage;

long() {
    write(
"This is the REAL Crypt of Kaunarl, the court magician.  The walls are\n"+
"decorated in a simple fashion -- a few of his more memorable achievements\n"+
"are depicted.  The room seems barren, but you feel an incredible magical\n"+
"presence.\n");
    if (!call_other("players/ringer/pyramid/nbalance","query_nstate")) {
        write("The openning to the north is blocked by a large stone slab.\n");
    }
    if (passage) write("A passage leads south.\n");
    write("You notice some symbols on the walls.\n");
}

reset() {
    set_light(1);
    passage=0;
}

south() {
    if (!passage) return 0;
    if (present("ringer_lich")) {
        write("The floating skull of Kaunarl prevents you!\n");
        return 1;
    }
    call_other(this_player(),"move_player",
               "south#players/ringer/pyramid/kaunarltr");
    return 1;
}

init() {
    add_action("search"); add_verb("search");
    add_action("north"); add_verb("north");
    add_action("south"); add_verb("south");
    add_action("look"); add_verb("look");
    add_action("look"); add_verb("l");
    add_action("read"); add_verb("read");
    add_action("read"); add_verb("exa");
}

north() {
    if (!call_other("players/ringer/pyramid/nbalance","query_nstate")) {
        write("The passage is blocked!\n");
        return 1;
    }
    call_other(this_player(),"move_player",
               "north#players/ringer/pyramid/sbalance");
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
    if (str != "wall" && str!="writings" && str!="walls")
      return 0;
    say(call_other(this_player(),"query_name")
        +" reads the wall.  A strange quality enters his voice.\n");
    if (present("amulet of tongues",this_player()) ||
        present("onyx amulet",this_player())) {
        write("\nThe onyx amulet glows brightly!\n");
        write(
"You read the writing on the wall, slowly pronouncing the strange words.\n"+
"You begin to feel dizzy and entranced.  You cannot stop reading and you\n"+
"find you voice growing in volume. Finally, your voice crests as you get\n"+
"to the last word in the spell!\n");
        tell_room(this_object(),
"\nA door flings open to the south!\n\n");
        tell_room(this_object(),
"Great maniacal laughter fills the room and a voice booms:\n"+
"'WHO DARES SUMMON KAUNARL?'\n"+
"A floating skull appears!  Its eyes resemble glowing yellow embers.\n");
        make_skull();
        passage=1;
    } else {
        write("There are words but the language is foreign to you.\n");
    }
    return 1;
}

look(str) {
    if (str != "at wall" && str != "at writings" && str != "at walls") 
      return 0;
    read("wall");
    return 1;
}

object ob,skull,item;

make_skull() {
    int t;
    
    t = random(5);
    ob = this_player();
    skull = clone_object("players/ringer/blink_dog");
    call_other(skull,"set_name","skull");
    call_other(skull,"set_alias","ringer_lich");
    call_other(skull,"set_short","A floating skull");
    call_other(skull,"set_long",
"The floating skull appears to be the magically animated remains of.\n"+
"Kaunarl.  Its eyes resemble glowing yellow embers.\n");
    call_other(skull,"set_level",10+t);
    call_other(skull,"set_ac",3+t);
    call_other(skull,"set_wc",8+t);
    call_other(skull,"set_aggressive",1);
    call_other(skull,"set_chance",30);
    call_other(skull,"set_spell_mess1",
"The skull's eyes flash brightly!");
    call_other(skull,"set_spell_mess2","You are hit with a searing flash!");
    call_other(skull,"set_spell_dam",30);
    call_other(skull,"set_align",-400);
    move_object(skull,this_object());
    item = clone_object("players/ringer/mitem");
    call_other(item,"set_id","jasper");
    call_other(item,"set_real_id","small jasper");
    call_other(item,"set_alias","gem");
    call_other(item,"set_short","A small jasper gem");
    call_other(item,"set_value",400);
    call_other(item,"set_weight",2);
    move_object(item,skull);
    item = clone_object("players/ringer/mitem");
    call_other(item,"set_id","topaz");
    call_other(item,"set_real_id","small topaz");
    call_other(item,"set_alias","gem");
    call_other(item,"set_short","A small topaz gem");
    call_other(item,"set_value",400);
    call_other(item,"set_weight",2);
    move_object(item,skull);
    call_other(skull,"attack_object",ob);
}
