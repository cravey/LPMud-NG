reset(arg)
{
    int i;
    object item,drag;
    set_light(1);
    if (!present("platinum dragon")) {
        drag = clone_object("obj/monster");
        call_other(drag,"set_name","platinum dragon");
        call_other(drag,"set_race","dragon");
        call_other(drag,"set_alias","ringer_pt_dragon");
        call_other(drag,"set_short","The platinum dragon");
        call_other(drag,"set_long","The dragon is very large.  His scales look like they are made of polished\n"
                   +"platinum.  He is clearly the king of good dragons.\n");
        call_other(drag,"set_level",19);
        call_other(drag,"set_ac",5000);
        call_other(drag,"set_wc",20);
        call_other(drag,"set_hp",1000);
        call_other(drag,"set_al",1000);
        call_other(drag,"set_aggressive",1);
        call_other(drag,"set_chance",20);
        call_other(drag,"set_spell_mess1",
"Platinum dragon breathes a tremendous stream of fire!\n");
        call_other(drag,"set_spell_mess2",
"Platinum dragon breathes a tremendous stream of fire!\n");
        call_other(drag,"set_spell_dam",50);
        move_object(drag,this_object());
        item = clone_object("players/ringer/mitem");
        call_other(item,"set_id","platinum crown");
        call_other(item,"set_alias","crown");
        call_other(item,"set_weight",5);
        call_other(item,"set_value",1000);
        call_other(item,"set_short","Platinum crown");
        call_other(item,"set_long",
"An exquisitely hand crafted crown.  Saphires and rubies add to the great\n"
+"beauty of the brilliant polished platinum.\n");
        call_other(item,"set_wear",
"This crown is dragon sized!  It will never fit you.\n");
        move_object(item,drag);
    } else 
      call_other(present("platinum dragon"),"set_ac",5000);
    i=0;
    if (!present("gold dragon")) {
        while(i<3) {
            drag = clone_object("obj/monster");
            call_other(drag,"set_name","gold dragon");
            call_other(drag,"set_race","dragon");
            call_other(drag,"set_alias","ringer_pt_gold_dragon");
            call_other(drag,"set_short","A gold dragon");
            call_other(drag,"set_long",
"The dragon is very large.  His scales look like they are made of the finest\n"
                       +"gold.\n");
            call_other(drag,"set_level",16);
            call_other(drag,"set_ac",30);
            call_other(drag,"set_wc",16);
            call_other(drag,"set_hp",300);
            call_other(drag,"set_al",900);
            call_other(drag,"set_aggressive",1);
            call_other(drag,"set_chance",20);
            call_other(drag,"set_spell_mess1",
                       "Gold dragon breathes a great stream of fire!\n");
            call_other(drag,"set_spell_mess2",
                       "Gold dragon breathes a great stream of fire!\n");
            call_other(drag,"set_spell_dam",30);
            move_object(drag,this_object());
            i=i+1;
        }  
    }
}

init()
{
    add_action("south"); add_verb("south");
    add_action("north"); add_verb("north");
}

south()
{
    if (!call_other(this_player(),"query_ghost")) {
        write("A strong magical force prevents you!\n");
        say(call_other(this_player(),"query_name") + "tries to go south and fails.\n");
        if (present("platinum dragon")) {
            tell_object(this_player(),
"Platinum dragon says: Oh no, you started this fight and we're going to\n"
+"finish it!\n");
            say(
"Platinum dragon says: Oh no, you started this fight and we're going to\n"
+"finish it!\n");
        }
        return 1;
    }
    call_other(this_player(),"move_player","south#players/ringer/pdhall");
    return 1;
}

north()
{
    if (present("gold dragon") || present("platinum dragon")) {
        write("A dragon blocks your way!\n");
        return 1;
    }
    call_other(this_player(),"move_player", "north#players/ringer/pdtr");
    return 1;
}

long()
{
    write("This is the platinum dragon's lair!  It is furnished with\n");
    write("elaborate silks and magical items.  There is a small room\n");
    write("to the north.\n");
}

short() {
    return "The platinum dragon's lair";
}

