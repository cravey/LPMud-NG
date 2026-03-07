
int s,e,mn;
object priest,item;

reset() {
    s = 0;
    e = 0;
    mn = 0;
    set_light(1);
    if (!present("pagan priest")) {
        priest = clone_object("obj/monster");
        call_other(priest,"set_name","pagan priest");
        call_other(priest,"set_alias","priest");
        call_other(priest,"set_level",5);
        call_other(priest,"set_wc",10);
        call_other(priest,"set_ac",3);
        call_other(priest,"set_long",
                   "The Priest is not happy you are here.\n");
        call_other(priest,"set_aggressive",1);
        call_other(priest,"set_al",-150);
        move_object(priest,this_object());
        item = clone_object("obj/weapon");
        call_other(item,"set_name","ceremonial sword");
        call_other(item,"set_alias","sword");
        call_other(item,"set_class",10);
        call_other(item,"set_short","A ceremonial sword");
        call_other(item,"set_value",150);
        call_other(item,"set_weight",2);
        move_object(item,priest);
    }
    if (!present("accolyte")) {
        priest = clone_object("obj/monster");
        call_other(priest,"set_name","accolyte");
        call_other(priest,"set_alias","assistant");
        call_other(priest,"set_level",4);
        call_other(priest,"set_wc",7);
        call_other(priest,"set_ac",3);
        call_other(priest,"set_long",
                   "The accolyte looks at you evilly.\n");
        call_other(priest,"set_aggressive",1);
        call_other(priest,"set_al",-150);
        move_object(priest,this_object());
        item = clone_object("obj/weapon");
        call_other(item,"set_name","ceremonial dagger");
        call_other(item,"set_alias","dagger");
        call_other(item,"set_class",7);
        call_other(item,"set_short","A ceremonial dagger");
        call_other(item,"set_value",50);
        call_other(item,"set_weight",2);
        move_object(item,priest);
    }
}


short() { return "A large round temple"; }

long() {
    write(
"A huge round temple surrounds you.  On the east wall is a large idol \n"+
"that seems devoted to the sun.  To the west a smililar one is devoted \n"+
"to the moon.  The largest idol is on the north wall and seems to be \n"+
"devoted to the earth.  The ceiling is black except where stars in various\n"+
"constellations have been painted.\n");
    if (!call_other("players/ringer/pyramid/wtempgate","query_ndoor")) 
        write("A door to the southwest is closed.\n");
    else
        write("A door to the southwest is open.\n");
    if (!call_other("players/ringer/pyramid/wtempgate","query_endoor")) 
        write("A door to the southeast is closed.\n");
    else
        write("A door to the southeast is open.\n");
    if (e) write("A passage leads north.\n");
    if (s) write("A passage leads east.\n");
    if (mn) write("A passage leads west.\n");
}

east() {
    if (!s) return 0;
    call_other(this_player(),"move_player",
               "east#players/ringer/pyramid/ehall1");
    return 1;
}

west() {
    if (!mn) return 0;
    call_other(this_player(),"move_player",
               "west#players/ringer/pyramid/whall1");
    return 1;
}

north() {
    if (!e) return 0;
    call_other(this_player(),"move_player",
               "north#players/ringer/pyramid/nhall1");
    return 1;
}

init() {
    add_action("search"); add_verb("search");
    add_action("north"); add_verb("north");
    add_action("east"); add_verb("east");
    add_action("west"); add_verb("west");
    add_action("southwest"); add_verb("southwest");
    add_action("southeast"); add_verb("southeast");
    add_action("move"); add_verb("move");
    add_action("move"); add_verb("push");
}

southwest() {
    if (!call_other("players/ringer/pyramid/wtempgate","query_ndoor")) {
        write("The door to the southwest is closed.\n");
        return 1;
    }
    call_other(this_player(),"move_player",
               "southwest#players/ringer/pyramid/wtempgate");
    return 1;
}

southeast() {
    if (!call_other("players/ringer/pyramid/wtempgate","query_endoor")) {
        write("The door to the southeast is closed.\n");
        return 1;
    }
    call_other(this_player(),"move_player",
               "southeast#players/ringer/pyramid/etempgate");
    return 1;
}

search(str) {
    if (!str) str = "room";
    if (str!="room") return 0;
    if (present("priest") || present("accolyte")) {
        write("The residents discourage that activity.\n");
        return 1;
    }
    write(
"A casual search reveals that the idols are on runners.  They seem\n"+
"moveable.  It seems strange that this is so obvious considering the \n"+
"workmanship you've seen so far.\n");
    say(call_other(this_player(),"query_name") + " searches the room.\n");
    return 1;
}

move(str) {
    if (present("priest") || present("accolyte")) {
        write("The residents discourage that activity.\n");
        return 1;
    }
    if (str=="idol") {
        write("Which idol? The sun idol, the moon idol, or the earth idol?\n");
        return 1;
    }
    if (str=="earth idol") {
        say(call_other(this_player(),"query_name")+" moves the "+str+".\n");
        write("The idol slides easily to one side revealing a passage.\n");
        e = 1;
        return 1;
    }
    if (str == "sun idol") {
        say(call_other(this_player(),"query_name")+" moves the "+str+".\n");
        write("The idol slides easily to one side revealing a passage.\n");
        s = 1;
        return 1;
    }
    if (str == "moon idol") {
        say(call_other(this_player(),"query_name")+" moves the "+str+".\n");
        write("The idol slides easily to one side revealing a passage.\n");
        mn = 1;
        return 1;
    }
}
