object item;

use(str) {
    object ob,obn,c;
    say(call_other(this_player(),"query_name") + " uses the scepter.\n");
    ob = first_inventory(environment(this_player()));
    while (ob) {
        obn = next_inventory(ob);
        c=present("mute_curse",ob);
        if (c) {
            destruct(c);
            tell_object(ob,"You can speak!\n");
        }
        ob = obn;
    }
    call_other(environment(this_player()),"rm_curse");
    return 1;
}

reset() { 
    if (!present("money")) {
        item = clone_object("obj/money");
        call_other(item,"set_money",random(500)+400);
        move_object(item,this_object());
    }
    if (!present("pulsating crystal")) {
        item = clone_object("players/ringer/spitem");
        call_other(item,"set_id","pulsating crystal");
        call_other(item,"set_real_id","crystal of enhancement");
        call_other(item,"set_charges",random(8)+7);
        call_other(item,"set_spell",5);
        call_other(item,"set_object","players/ringer/pyramid/kaunarltr");
        call_other(item,"set_alias","crystal");
        call_other(item,"set_short","A small pulsating crystal");
        call_other(item,"set_value",200);
        call_other(item,"set_weight",1);
        move_object(item,this_object());
    }
    if (!present("dark staff")) {
        item = clone_object("players/ringer/spitem");
        call_other(item,"set_id","dark staff");
        call_other(item,"set_real_id","staff of fire");
        call_other(item,"set_charges",5);
        call_other(item,"set_spell",4);
        call_other(item,"set_alias","staff");
        call_other(item,"set_short","A dark staff");
        call_other(item,"set_value",100);
        call_other(item,"set_weight",1);
        move_object(item,this_object());
    }
    if (!present("green orb")) {
        item = clone_object("players/ringer/spitem");
        call_other(item,"set_id","green orb");
        call_other(item,"set_real_id","orb of healing");
        call_other(item,"set_charges",4);
        call_other(item,"set_spell",5);
        call_other(item,"set_object","players/ringer/pdtr");
        call_other(item,"set_alias","orb");
        call_other(item,"set_short","A green glowing orb");
        call_other(item,"set_value",100);
        call_other(item,"set_weight",1);
        move_object(item,this_object());
    }
    if (!present("scepter of Xyphelod")) {
        item = clone_object("players/ringer/spitem");
        call_other(item,"set_id","scepter of Xyphelod");
        call_other(item,"set_charges",1);
        call_other(item,"set_spell",5);
        call_other(item,"set_object",this_object());
        call_other(item,"set_alias","scepter");
        call_other(item,"set_short","The scepter of Xyphelod");
        call_other(item,"set_value",400);
        call_other(item,"set_weight",1);
        move_object(item,this_object());
    }
}


short() { return "Treasure room of Xyphelod"; }

long() {
    write(
"This is the royal treasure room.  Here all the wealth of the royal\n"+
"family lies.\n");
}

init() {
    add_action("search"); add_verb("search");
    add_action("north"); add_verb("north");
    add_action("south"); add_verb("south");
}

north() {
    call_other(this_player(),"move_player",
               "north#players/ringer/pyramid/xycrypt");
    return 1;
}

south() {
    call_other(this_player(),"move_player",
               "south#players/ringer/pyramid/pyr1");
    return 1;
}

search(str) {
    if (!str) str = "room";
    if (str == "room") {
        write(
"As you search, you attempt to touch the south wall.  Your hand goes\n"+
"it!\n");
        say(call_other(this_player(),"query_name") + " searches the room.\n");
        return 1;
    }
}
