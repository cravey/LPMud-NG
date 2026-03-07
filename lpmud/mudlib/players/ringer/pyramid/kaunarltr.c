object item;

reset() { 
    if (!present("money")) {
        item = clone_object("obj/money");
        call_other(item,"set_money",random(300)+200);
        move_object(item,this_object());
    }
    if (!present("pulsating crystal")) {
        item = clone_object("players/ringer/spitem");
        call_other(item,"set_id","pulsating crystal");
        call_other(item,"set_real_id","crystal of enhancement");
        call_other(item,"set_charges",random(4)+2);
        call_other(item,"set_spell",5);
        call_other(item,"set_object",this_object());
        call_other(item,"set_alias","crystal");
        call_other(item,"set_short","A small pulsating crystal");
        call_other(item,"set_value",200);
        call_other(item,"set_weight",1);
        move_object(item,this_object());
    }
}

use(str) {
    object t;
    string s,s1;
    if (!str) { write("Try using on something.\n"); return 1;}
    t = present(str,environment(this_player()));
    if (!t) t = present(str,this_player());
    if (!t) {
        write("Use on what?\n");
        return 0;
    }
    s = call_other(t,"query_name");
    if (!s) s = call_other(t,"short");
    if (sscanf(s,"A %s",s1)==1) s="the "+s1;
    write("A beam of soft purple light envelopes "+s+".\n");
    say(call_other(this_player(),"query_name") + " uses a crystal.\n");
    say("A beam of soft purple light envelopes "+s+".\n");
    if (living(t)) {
        return enhance_living(t);
    }
    if (call_other(t,"query_type")) {
        return enhance_armor(t);
    }
    if (call_other(t,"weapon_class")) {
        return enhance_weapon(t);
    }
    /* If not, try to up charges.... */
    if (call_other(t,"query_magic")) {
        return enhance_magic(t);
    }
    write("You feel vaguely disappointed.\n");
    return 0;
}

enhance_magic(t) {
    int chg;
    string s,s1,s2;
    chg = call_other(t,"query_charges");
    chg += 5;
    s = call_other(t,"query_id");
    if (sscanf(s,"%senhancement",s1,s2)) 
      chg = 25;
    call_other(t,"set_charges",chg);
    if (!call_other(t,"query_charges")) {
        write("You feel vaguely disappointed.\n");
        return 0;
    }
    if (random(100) > 100-chg*3 || chg > 20) { 
        write(call_other(t,"short") + " glows violently green!\n");
        write("Its brilliance continues to grow more and more!\n");
        say(call_other(t,"short") + " glows violently green!\n");
        say("Its brilliance continues to grow more and more!\n");
        write(call_other(t,"short")
              +" explodes in a maelstrom of magical plasma!\n");
        write("The room and all in it are engulfed in the explosion.\n");
        say(call_other(t,"short")
            +" explodes in a maelstrom of magical plasma!\n");
        say("The room and all in it are engulfed in the explosion.\n");
        call_other(t,"magstorm");
        destruct(t);
    }
    return 1;
}


enhance_living(t) {
    int lvl;
    lvl = call_other(t,"query_level");
    call_other(t,"heal_self",random(lvl*4)+5);
    return 1;
}

enhance_armor(t) {
    int ac,max;
    string type;
    ac = call_other(t,"armor_class");
    type = call_other(t,"query_type");
    max = 2;
    if (type=="armor") max=5;
    if (type=="ring") max=1;
    ac += 1;
    if (ac > max || random(100)>80-ac*4) {
        write(call_other(t,"short") + " glows violently green!\n");
        write("It looks like something got overloaded.\n");
        write("You are hit by the backlash!\n");
        say(call_other(t,"short") + " glows violently green!\n");
        say("It looks like something got overloaded.\n");
        call_other(this_player(),"hit_player",random(ac*5));
        ac -=3;
    }
    call_other(t,"set_ac",ac);
    return 1;
}

enhance_weapon(t) {
    int wc,max;
    wc = call_other(t,"weapon_class");
    max = 21;
    wc += (1 + random(2));
    if (wc > max || random(100)>80-wc) {
        write(call_other(t,"short") + " glows violently green!\n");
        write("It looks like something got overloaded.\n");
        write("You are hit by the backlash!\n");
        say(call_other(t,"short") + " glows violently green!\n");
        say("It looks like something got overloaded.\n");
        call_other(this_player(),"hit_player",random(wc*3/2));
        wc -= 6;
    }
    call_other(t,"set_class",wc);
    return 1;
}


short() { return "Treasure room of Kaunarl"; }

long() {
    write(
"This is the treasure room of Kaunarl, the court magician.  Here all his\n"+
"wealth and possessions were put at the time of his burial.\n"+
"You notice some writings on the wall.\n");
}

init() {
    add_action("search"); add_verb("search");
    add_action("north"); add_verb("north");
    add_action("look"); add_verb("look");
    add_action("look"); add_verb("l");
    add_action("read"); add_verb("read");
    add_action("read"); add_verb("exa");
}

north() {
    call_other(this_player(),"move_player",
               "north#players/ringer/pyramid/mgcrypt");
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
        +" reads the wall.\n");
    if (present("amulet of tongues",this_player()) ||
        present("onyx amulet",this_player())) {
        write("\nThe onyx amulet glows brightly!\n");
        write(
"Beware, lest the curse of Xyphelod be upon you!  Behold the portal to\n"+
"his crypt!  Listen, tarry the proper number of clicks and step through\n"+
"if you would incur his wrath!\n");
    } else {
        write("There are words but the language is foreign to you.\n");
    }
    write("There are also some strange symbols.\n");
    write("           | - - |\n");
    write("o x O X    |  O  |\n\n");
    write(",. | ,, | ,., | | ,.,,\n");
    return 1;
}

look(str) {
    if (str != "at wall" && str != "at writings" && str != "at walls") 
      return 0;
    read("wall");
    return 1;
}

