#include "../../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT ex_init();

#undef EXTRA_RESET
#define EXTRA_RESET ex_reset();

ONE_EXIT("players/ringer/pyramid/desert8","exit",
          "A large plush hall",
"This is a large plush hall.  Not at all what you would expect to find\n"+
"in the middle of the desert.  You have the distinct impression that\n"+
"something magical is responsible.\n",1)

object g1,g,ball;

ex_reset() {
    if (!present("crystal ball")) {
        ball = clone_object("obj/treasure");
        call_other(ball,"set_id","crystal ball");
        call_other(ball,"set_alias","ball");
        call_other(ball,"set_short","A red crystal ball");
        call_other(ball,"set_long",
"Inside the ball you can discern the small figure of a girl about 4 inches\n"+
"tall.  She is clearly distressed.  She pounds on the glass.  It seems like\n"+
"she wants to tell you something.\n");
        call_other(ball,"set_weight",500);
        move_object(ball,this_object());
    }
    if (!present("sand fairy")) {
        g = clone_object("obj/monster.talk.c");
        call_other(g,"set_name","sand fairy");
        call_other(g,"set_alias","fairy");
        call_other(g,"set_short","A sand fairy");
        call_other(g,"set_level",8);
        call_other(g,"set_ac",4);
        call_other(g,"set_wc",8);
        call_other(g,"set_long",
"The sand fairy is very lovely.  She is dressed nicely in light brown.\n");
        call_other(g,"set_aggressive",0);
        call_other(g,"set_chance",30);
        call_other(g,"set_spell_mess1",
                   "Sand fairy summons a blast of sand.\n");
        call_other(g,"set_spell_mess2",
                   "Sand fairy summons a blast of sand.\n");
        call_other(g,"set_spell_dam",20);
        call_other(g,"set_al",-600);
        call_other(g,"set_object",this_object());
        call_other(g,"set_function","greet");
        call_other(g,"set_type","says:");
        call_other(g,"set_match"," "); 
        call_other(g,"set_object",this_object());
        call_other(g,"set_function","arrive");
        call_other(g,"set_type","arrives");
        call_other(g,"set_match","");  
        move_object(g,this_object());
        g1 = present("goblin shaman");
        if (g1) {
            destruct(g1);
        }
        g1 = present("scepter of Xyphelod");
        if (g1) {
            destruct(g1);
        }
    }
}

arrive(str) {
    string who;
    object ob;

    if (sscanf(str,"%s arrives.",who)!=1) return;
    ob = present(lower_case(who),this_object());
    if (!ob) return;
    if (present("scepter of Xyphelod",ob)) {
        talk(
"The sand fairy says: Quick! Give me the scepter!  It is the only way to \n"+
"dispell the curse and free the princess!\n");
    }
}


greet(str) {
    int r;
    string who,what;

    if (sscanf(str,"%sprincess%s",who,what)>0 ||
        sscanf(str,"%sprisoner%s",who,what)>0 ||
        sscanf(str,"%squest%s",who,what)>0 ||
        sscanf(str,"%scaptive%s",who,what)>0 ) {
        talk(
"\nThe sand fairy says: Oh! At last someone has come to help us!  Surely\n"+
"you are on a quest to free my princess and I!  The princess Cithina and\n"+
"I have been trapped by the evil sand goblins.  The evil goblin shaman\n");
        talk(
"cast a spell to seal her in this crystal ball.  I fought him with my magic\n"+
"and I vanquished him, but my princess is still trapped in the ball.  Only\n");
        talk(
"the scepter of Xyphelod has the power to release her.  Bring it to me and\n"+
"I will be able to free the princess.\n");
        return 1;
    }
    if (sscanf(str,"%sscepter%s",who,what)>0 ||
        sscanf(str,"%sxyphelod%s",who,what)>0 ||
        sscanf(str,"%swhere%s",who,what)>0) {
        talk(
"\nThe sand fairy says: Xyphelod was the great king of the desert.  His\n"+
"crypt is somewhere in the sands of the desert.  I believe his scepter\n"+
"was buried with him.\n");
        return 1;
    }
}

talk(str) {
    write(str);
    if (g) say(str,g); else say(str);
}

ex_init() {
    add_action("search"); add_verb("search");
    add_action("give"); add_verb("give");
}

search(str) {
    if (!str) str = "room";
    if (str == "room") {
        write("You find nothing unusual.\n");
        say(call_other(this_player(),"query_name") + " searches the room.\n");
        return 1;
    }
}

give(str) {
    object ob,obm;
    string what,who,mess;
    if (sscanf(str,"%s to %s",what,who)!=2) return 0;
    if (who !="fairy" && who!="sand fairy") return 0;
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


object att;
int cnt;

gve(str) {
    string who,rest,item,whom;
    object ob;

    sscanf(str,"%s gives %s",who,rest);
    if (!rest) return;
    sscanf(rest,"%s to %s.",item,whom);
    if (!call_other(g,"id",lower_case(whom))) return;
    ob = present(lower_case(item),g);
    if (!ob) return;
    if (call_other(ob,"id","staff of Xyphelod")) {
        talk(
"The sand fairy examines the staff.  She sighs and shakes her head.\n"+
"The sand fairy says: No, I am sorry.  This is not the scepter of Xyphelod.\n"+
"This is a cheap imitation.  You must have found it in a false crypt.  You\n"+
"must look deeper to find the real scepter.\n");
        destruct(ob);
        return;
    }
    if (call_other(ob,"id","scepter of Xyphelod")) {
        talk(
"The sand fairy snatches the scepter from your hand!  She laughs evily!\n"+
"She is transformed before your eyes into a knarled, ugly, old, goblin\n"+
"shaman!\n");
        talk(
"The shaman says: You fool!  Now I have the power to destroy you and all\n"+
"who would oppose me!  Did you really believe that sickly sweet story about\n"+
"the poor trapped sand fairy?  I trapped the princess to get some chump\n"+
"to fetch the scepter for me and you did it! HAHAHAHAHAHAHAHA!\n");
        g1 = g;
        g = clone_object("obj/monster");
        call_other(g,"set_name","goblin shaman");
        call_other(g,"set_alias","shaman");
        call_other(g,"set_race","goblin");
        call_other(g,"set_short","A sand goblin shaman");
        call_other(g,"set_level",19);
        call_other(g,"set_hp",700);
        call_other(g,"set_ac",10);
        call_other(g,"set_wc",18);
        call_other(g,"set_long",
"The goblin shaman is old, ugly, and evil.  He looks invincible.\n");
        call_other(g,"set_aggressive",1);
        call_other(g,"set_chance",30);
        call_other(g,"set_spell_mess1",
                   "Shaman summons a blast of sand.\n");
        call_other(g,"set_spell_mess2",
                   "Shaman summons a blast of sand.\n");
        call_other(g,"set_spell_dam",40);
        call_other(g,"set_al",-600);
        move_object(g1,g);
        destruct(g1);
        move_object(g,this_object());
        att = present(lower_case(who));
        cnt=0;
        set_heart_beat(1);
    }
}


heart_beat() {
    cnt+=1;
    if (cnt==2) {
        say("The goblin shaman sucks the magic from the scepter!\n");
        return;
    }
    if (cnt==3) {
        say("The goblin shaman looks omnipotent!\n");
        return;
    }
    if (cnt <6) 
      return;
    call_other(g,"attack_object",att);
    set_heart_beat(0);
}

rm_curse() {
    if (!ball) return 0;
    att = this_player();
    talk("The crystal ball evaporates and a lovely princess appears!\n");
    destruct(ball);
    destruct(present("scepter",this_player()));
    talk(
"She kisses you on the cheek and whispers:  Well done. Your quest is\n"+
"finished.\n"+
"She turns a ring on her finger and is gone in a shimmering of glitter.\n");
    call_other(this_player(),"add_exp",100000);
    call_other(this_player(), "set_quest", "ringer");
    if (present("sand fairy")) {
        talk(
"The sand fairy looks furious!  Suddenly she is transformed into a knarled,\n"+
"ugly, old goblin shaman!\n");
        talk(
"The shaman says:  You swine!  How did you see through my deception?  Now\n"+
"give me the pleasure of destroying you!\n");
        g1 = g;
        g = clone_object("obj/monster");
        call_other(g,"set_name","goblin shaman");
        call_other(g,"set_alias","shaman");
        call_other(g,"set_race","goblin");
        call_other(g,"set_short","A sand goblin shaman");
        call_other(g,"set_level",10);
        call_other(g,"set_ac",4);
        call_other(g,"set_wc",9);
        call_other(g,"set_long",
"The goblin shaman is old, ugly, and evil.\n");
        call_other(g,"set_aggressive",1);
        call_other(g,"set_chance",30);
        call_other(g,"set_spell_mess1",
                   "Shaman summons a blast of sand.\n");
        call_other(g,"set_spell_mess2",
                   "Shaman summons a blast of sand.\n");
        call_other(g,"set_spell_dam",20);
        call_other(g,"set_al",-600);
        move_object(g1,g);
        destruct(g1);
        move_object(g,this_object());
        cnt=3;
        set_heart_beat(1);
    }
}
        
