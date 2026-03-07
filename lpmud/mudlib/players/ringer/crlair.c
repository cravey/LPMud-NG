reset(arg)
{
    int i;
    object item,drag;
    set_light(1);
    if (!present("chromatic dragon")) {
        drag = clone_object("obj/monster");
        call_other(drag,"set_name","chromatic dragon");
        call_other(drag,"set_race","dragon");
        call_other(drag,"set_alias","ringer_cr_dragon");
        call_other(drag,"set_short","The chromatic dragon");
        call_other(drag,"set_long","The dragon is very large.  Her scales look like they are made of \n"
 +"polished chromium.  She is clearly the queen of evil dragons.\n");
        call_other(drag,"set_level",19);
        call_other(drag,"set_ac",5000);
        call_other(drag,"set_wc",20);
        call_other(drag,"set_hp",1000);
        call_other(drag,"set_al",-1000);
        call_other(drag,"set_aggressive",1);
        call_other(drag,"set_chance",20);
        call_other(drag,"set_spell_mess1",
"Chromatic dragon breathes a tremendous stream of fire!\n");
        call_other(drag,"set_spell_mess2",
"Chromatic dragon breathes a tremendous stream of fire!\n");
        call_other(drag,"set_spell_dam",50);
        move_object(drag,this_object());
        item = clone_object("players/ringer/mitem");
        call_other(item,"set_id","chromium crown");
        call_other(item,"set_alias","crown");
        call_other(item,"set_weight",5);
        call_other(item,"set_value",1000);
        call_other(item,"set_short","Chromium crown");
        call_other(item,"set_long",
"An exquisitely hand crafted crown.  Diamonds and pearls add to the great\n"
+"beauty of the brilliant polished chromium.\n");
        call_other(item,"set_wear",
"This crown is dragon sized!  It will never fit you.\n");
        move_object(item,drag);
    } else 
      call_other(present("chromium dragon"),"set_ac",5000);
    i=0;
    if (!present("court demon")) {
        while(i<3) {
            drag = clone_object("obj/monster");
            call_other(drag,"set_name","court demon");
            call_other(drag,"set_alias","ringer_cr_demon");
            call_other(drag,"set_race","demon");
            call_other(drag,"set_short","Court demon");
            call_other(drag,"set_long",
"The demon is large and ugly.  Red glowing eyes look menacingly at you.\n");
            call_other(drag,"set_level",16);
            call_other(drag,"set_ac",30);
            call_other(drag,"set_hp",300);
            call_other(drag,"set_wc",16);
            call_other(drag,"set_al",-900);
            call_other(drag,"set_aggressive",1);
            call_other(drag,"set_chance",20);
            call_other(drag,"set_spell_mess1",
                       "Court demon hisses.\n");
            call_other(drag,"set_spell_mess2",
                       "Court demon hisses.\n");
            call_other(drag,"set_spell_dam",30);
            move_object(drag,this_object());
            i=i+1;
        }  
    }
}

init()
{
    add_action("east"); add_verb("east");
    add_action("west"); add_verb("west");
}

west()
{
    if (!call_other(this_player(),"query_ghost")) {
        write("A strong magical force prevents you!\n");
        say(call_other(this_player(),"query_name") + "tries to go south and fails.\n");
        if (present("chromatic dragon")) {
            tell_object(this_player(),
"Chromatic dragon says: Oh no, you started this fight and we're going to\n"
+"finish it!\n");
            say(
"Chromatic dragon says: Oh no, you started this fight and we're going to\n"
+"finish it!\n");
        }
        return 1;
    }
    call_other(this_player(),"move_player","north#players/ringer/cr3");
    return 1;
}

east()
{
    if (present("dragon")) {
        write("Chromatic dragon blocks your way!\n");
        return 1;
    }
    if (present("demon")) {
        write("A demon blocks your way!\n");
        return 1;
    }
    call_other(this_player(),"move_player", "north#players/ringer/crtr");
    return 1;
}

long()
{
    write("This is the chromatic dragon's lair!  It is furnished with\n");
    write("slime and damp moss and mold.  It is sickly evil and you feel\n");
    write("REAL uncomfortable.\n");
}

short() {
    return "The chromatic dragon's lair";
}

