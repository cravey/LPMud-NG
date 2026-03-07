#include "../../room/std.h"

#undef EXTRA_RESET
#define EXTRA_RESET ex_reset();

#undef EXTRA_INIT
#define EXTRA_INIT ex_init();

TWO_EXIT("players/ringer/en_forest", "east",
         "players/ringer/en_for3","west",
	   "You are in a calm quiet forest",
"You are in a calm quiet forest. Sunlight streams through the trees above.\n"
+"You feel great peace and solitude.  There is a stump here.\n",1)

ex_init() {
    add_action("give"); add_verb("give");
}

give(str) {
    object ob,obm;
    string what,who,mess;
    if (sscanf(str,"%s to %s",what,who)!=2) return 0;
    if (who !="master" && who!="quest master") return 0;
    obm = present(who,environment(this_player()));
    ob=present(what,this_player());
    if (!ob || !obm) return 0;
    write("Ok.\n");
    mess=call_other(this_player(),"query_name")+
        " gives "+what+" to "+who+".\n";
    say(mess);
    move_object(ob,obm);
    gve(mess);
    return 1;
}

object g;
ex_reset() {
    if (!present("master")) {
        g = clone_object("obj/monster.talk.c");
        call_other(g,"set_name","quest master");
        call_other(g,"set_alias","master");
        call_other(g,"set_level",8);
        call_other(g,"set_ac",6);
        call_other(g,"set_long",
"The quest master is sitting in some soft grass juggling fireballs.\n");
        call_other(g,"set_aggressive",0);
        call_other(g,"set_chance",30);
        call_other(g,"set_spell_mess1","Quest master summons a fireball.\n");
        call_other(g,"set_spell_mess2","Quest master summons a fireball.\n");
        call_other(g,"set_spell_dam",40);
        call_other(g,"set_al",0);
        call_other(g,"set_object",this_object());
        call_other(g,"set_function","greet");
        call_other(g,"set_type","says:");
        call_other(g,"set_match"," ");
        call_other(g,"set_object",this_object());
        call_other(g,"set_function","smile");
        call_other(g,"set_type","smiles");
        call_other(g,"set_match"," "); 
        call_other(g,"set_object",this_object());
        call_other(g,"set_function","kick");
        call_other(g,"set_type","kicks");
        call_other(g,"set_match","you.  OUCH!");
/*        call_other(g,"set_object",this_object());
        call_other(g,"set_function","give");
        call_other(g,"set_type","gives");
        call_other(g,"set_match"," "); */
        move_object(g,this_object());
    } 
}

string person,given,towhom;

gve(str) {
    string who,rest,item,whom;
    object ob,ob1,pl;

    sscanf(str,"%s gives %s",who,rest);
    if (!rest) return;
    sscanf(rest,"%s to %s.",item,whom);
    if (!call_other(g,"id",lower_case(whom))) return;
    ob = present(lower_case(item),g);
    if (!ob) return;
    if (call_other(ob,"id","platinum crown") ||
	call_other(ob,"id","chromium crown")) {
	talk("The quest master bows to "+who+".\n");
	talk("Quest master says: Congratulations on completing my quest.\n");
	pl = present(lower_case(who),environment(g));
	if (!pl) return;
	call_other(pl,"set_quest","ringer");
	destruct(ob);
	return;
    }
    if (call_other(ob,"id","black bleak blade of blood")) {
        ob1 = present("black orb",g);
        if (!ob1) ob1 = present("orb of missiles",g);
        if (!ob1) {
            talk(
"The quest master says: All I need now is the orb of the groo prince.\n");
            return;
        }
        pl = present(lower_case(who),environment(g));
        if (!pl) return;
        talk(
"The quest master says: Very good!  Now you'll be needing this so I'll give\n"
+"it back to you.\n");
        destruct(ob1);
talk("The quest master hands the black bleak blade of blood to "
    + lower_case(who)+"\n");
        move_object(ob,pl);
        talk(
"The quest master says: Oh, yes I should also give you this.  Might come in\n"
+"handy! (He grins)\n");
        ob = make_blast("gold dragon");
        talk("The quest master hands a black rod to "+lower_case(who)+"\n");
        move_object(ob,pl);
        talk(
"The quest master says: Now off you go and good luck!  He laughs maniacally\n"
+"for a moment as he waves his hands.\n");
        call_other(pl,"move_player","X#players/ringer/pdhall");
        return;
    }
    if (call_other(ob,"id","black orb")
        || call_other(ob,"id","orb of missiles")) {
        if (call_other(ob,"query_charges")<10) {
            talk(
"The quest master says: This thing has been used!  I want a fully charged\n"
+"orb!  This is useless!  He destroys it in a puff of smoke.\n");
            destruct(ob);
            return;
        }
        ob1 = present("black bleak blade of blood",g);
        if (!ob1) {
            talk(
"The quest master says: All I need now is the black bleak blade of blood.\n");
            return;
        }
        pl = present(lower_case(who),environment(g));
        if (!pl) return;
        talk(
"The quest master says: Very good!  Now you'll be needing this so I'll give\n"
+"it back to you.\n");
        destruct(ob);
talk("The quest master hands the black bleak blade of blood to "+lower_case(who)+"\n");
        move_object(ob1,pl);
        talk(
"The quest master says: Oh, yes I should also give you this.  Might come in\n"
+"handy! (He grins)\n");
        ob = make_blast("gold dragon");
        talk("The quest master hands a black rod to "+lower_case(who)+"\n");
        move_object(ob,pl);
        talk(
"The quest master says: Now off you go and good luck!  He laughs maniacally\n"
+"for a moment as he waves his hands.\n");
        call_other(pl,"move_player","X#players/ringer/pdhall");
        return;
    } 
    if (call_other(ob,"id","shining sword")) {
        ob1 = present("golden staff",g);
        if (!ob1) ob1 = present("staff of Gurfnel",g);
        if (!ob1) {
            talk(
"The quest master says: All I need now is the golden staff of Gurfnel.\n");
            return;
        }
        pl = present(lower_case(who),environment(g));
        if (!pl) return;
        talk(
"The quest master says: Very good!  Now you'll be needing this so I'll give\n"
+"it back to you.\n");
        destruct(ob1);
talk("The quest master hands the shining sword to "+lower_case(who)+"\n");
        move_object(ob,pl);
        talk(
"The quest master says: Oh, yes I should also give you this.  Might come in\n"
+"handy! (He grins)\n");
        ob = make_blast("court demon");
        talk("The quest master hands a blue rod to "+lower_case(who)+"\n");
        move_object(ob,pl);
        talk(
"The quest master says: Now off you go and good luck!  He laughs maniacally\n"
+"for a moment as he waves his hands.\n");
        call_other(pl,"move_player","X#players/ringer/cr1");
        return;
    }
    if (call_other(ob,"query_id")=="staff of Gurfnel") {
        if (call_other(ob,"query_charges")<5) {
            talk(
"The quest master says: This thing has been used!  I want a fully charged\n"
+"staff!  This is useless!  He destroys it in a puff of smoke.\n");
            destruct(ob);
            return;
        }
        ob1 = present("shining sword",g);
        if (!ob1) {
            talk(
"The quest master says: All I need now is the Gulf's shining sword.\n");
            return;
        }
        pl = present(lower_case(who),environment(g));
        if (!pl) return;
        talk(
"The quest master says: Very good!  Now you'll be needing this so I'll give\n"
+"it back to you.\n");
        destruct(ob);
talk("The quest master hands the shining sword to "+lower_case(who)+"\n");
        move_object(ob1,pl);
        talk(
"The quest master says: Oh, yes I should also give you this.  Might come in\n"
+"handy! (He grins)\n");
        ob = make_blast("court demon");
        talk("The quest master hands a blue rod to "+lower_case(who)+"\n");
        move_object(ob,pl);
        talk(
"The quest master says: Now off you go and good luck!  He laughs maniacally\n"
+"for a moment as he waves his hands.\n");
        call_other(pl,"move_player","X#players/ringer/cr1");
        return;
    } 
}


make_blast(str) {
    object ob;
    ob = clone_object("players/ringer/spitem");
    if (str=="dragon") {
        call_other(ob,"set_id","black rod");
        call_other(ob,"set_short","A black rod");
    } else {
        call_other(ob,"set_id","blue rod");
        call_other(ob,"set_short","A blue rod");
    }
    call_other(ob,"set_real_id","rod of "+str+" blasting");
    call_other(ob,"set_blast_dam",75);
    call_other(ob,"set_spell",6);
    call_other(ob,"set_value",100);
    call_other(ob,"set_charges",5);
    call_other(ob,"set_blast_id",str);
    call_other(ob,"set_alias","rod");
    return ob;
}


smile(str) {
    talk("\nThe quest master smiles happily.\n");
}

kick(str) {
    object ob,kicker;
    string who,rest;

    sscanf(str, "%s %s\n", who, rest);
    talk(
"Quest master, annoyed that someone would kick him, summons a demon.\n");
    ob = clone_object("players/ringer/blink_dog");
    call_other(ob,"set_name","quest demon");
    call_other(ob,"set_alias","demon");
    call_other(ob,"set_level",12);
    call_other(ob,"set_long","The demon is REAL nasty to look at!\n");
    call_other(ob,"set_wc",21);
    call_other(ob,"set_move_at_reset",0);
    call_other(ob,"set_aggressive",1);
    call_other(ob,"set_chance",25);
    call_other(ob,"set_spell_mess1","The demon screams");
    call_other(ob,"set_spell_mess2","The demon screams");
    call_other(ob,"set_spell_dam",35);
    call_other(ob,"set_al",-300);
    move_object(ob,this_object());
    kicker = present(lower_case(who));
    call_other(ob,"attack_object",kicker);
}

greet(str) {
    int r;
    string who,what;
    if (sscanf(str,"%shelp%s",who,what) || sscanf(str,"%shint%s",who,what) 
        || sscanf(str,"%stip%s",who,what)) {
        talk(
"\nThe quest master says: You want a hint?  Lets see...Here's one:\n");
        r = random(10)+1;
        if (r==1) what = "Only one blade will kill that dragon.\n";
        if (r==2) 
          what = "Sometimes the dead can do more than the living.";
        if (r==3) what = "Search and you shall find!";
        if (r==4) what = "When in doubt, identify!";
        if (r==5) what = "Be sociable! Bring some friends.";
        if (r==6) 
          what = "Trapped in a stone? Crushing it is the painful way out.";
        if (r==7) what = "There is something familiar about that monster...";
        if (r==8) 
          what = "The dragon's lair is entered easily, but leaving is tough.";
        if (r==9) what = "The chipmunk has some things to hide.";
        if (r==10) what = "Free fall is fun, but dangerous.";
        talk(what+"\n");
    }
    if (sscanf(str,"%splatinum%s",who,what)>0) {
        talk(
"\nThe quest master says: Oh, an evil one?  Ok, mister sinister, here's the\n"
+"deal:  I arrange transport to the platinum dragon's domain, but first you\n"
+"gather some things for me (I don't do this for nothing).  For this quest\n");
        talk(
"I need the orb of the groo prince and the black bleak blade of blood\n"
+"(no I'm not kidding).  Or, if you're good, you can find the dragon \n"
+"yourself.\n");
	talk(
"Give me the crown to complete my quest.\n");
        return 1;
    }
    if (sscanf(str,"%schromatic%s",who,what)>0) {
        talk(
"\nThe quest master says: You're an enemy of evil?  Ok, peter pureheart, this\n"
+"is the proposition: I'll get you near the chromatic dragon's lair, (its \n"
+"real hard to find!), but first there are some things I need.  Bring me the\n");
        talk(
"brilliant sword from Gulf the magician. Also, the servants of Gurfnel have\n"
+"a hidden treasure.  Bring me the golden staff. You could find the dragon\n"
+"on your own I suppose, but take my advice and gather this stuff.\n");
	talk(
"Give me the crown to complete my quest.\n");
        return 1;
    }
    if (sscanf(str,"%squest%s",who,what)>0) {
        talk(
"\nThe quest master says: You want a quest eh?  Lets see we got the\n"
+"chromatic dragon quest for the good guys and the platinum dragon hunt\n"
+"for the evil slime buckets.  Got a preference?\n");
        return 1;
    }
}

talk(str) {
    write(str);
    if (g) say(str,g); else say(str);
}

