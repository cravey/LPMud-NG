int i;
object demon;
reset() {
    i=0;
    if (!present("demon")) {
        while (i<5) {
            make_demon();
            i += 1;
        }
    }
}

make_demon() {
    int t,a,w,l;
    string name;
    t = random(4);
    if (t==0) {
        name="black demon";
        a = 3;
        w = 8;
        l = 7;
    }
    if (t==1) {
        name="pit fiend";
        a = 4;
        w = 10;
        l = 8;
    }
    if (t==2) {
        name="fire demon";
        a = 4;
        w = 14;
        l = 10;
    }
    if (t==3) {
        name="frost demon";
        a = 4;
        w = 17;
        l = 11;
    }
    demon = clone_object("obj/monster");
    call_other(demon, "set_name", name);
    call_other(demon, "set_alias", "demon");
    call_other(demon, "set_level", l);
    call_other(demon, "set_wc",w);
    call_other(demon, "set_ac",a);
    call_other(demon, "set_al", -160);
    call_other(demon, "set_short", "A "+name);
    call_other(demon, "set_aggressive", 1);
    move_object(demon,this_object());
}

short() { return "A demon pit"; }

long() {
    write(
"This is a deep demon pit.  Those who are sent here are not intended to \n"+
"LIVE.  In fact, they are intended to be shredded and eaten by nasty \n"+
"demons.\n");
    write("There is a glowing portal leading up.\n");
}

init() {
    add_action("search"); add_verb("search");
    add_action("up"); add_verb("up");
    add_action("up"); add_verb("climb");
}

up() {
    if (present("demon") && !call_other(this_player(),"query_ghost")) {
        write("The demons make it impossible to climb.\n");
        return 1;
    }
    call_other(this_player(),"move_player",
               "up#players/ringer/pyramid/desert4");
    return 1;
}

search(str) {
    if (!str) str = "room";
    if (str == "room") {
        say(call_other(this_player(),"query_name") + " searches the room.\n");
        write("You find nothing unusual.\n");
        return 1;
    }
}
