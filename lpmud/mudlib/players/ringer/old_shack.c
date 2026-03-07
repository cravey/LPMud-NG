#include "../../room/std.h"

#undef EXTRA_RESET
#define EXTRA_RESET\
        ex_reset();

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

#undef EXTRA_LONG
#define EXTRA_LONG\
       ex_long(str);

ONE_EXIT("players/ringer/dn_valley", "north",
	   "You are in an ugly old shack.",
"You are in an ugly old shack.  It looks dirty and broken down.\n",1)

object witch, raven, item;
int passage_open;

ex_init() {
    add_action("search"); add_verb("search");
    add_action("down"); add_verb("down");
    passage_open = 0;
}

ex_long(){
    if (passage_open) write("There is a trap door open leading down\n");
}

down(){
    if (!passage_open) return 0;
    if (present("witch")) { 
        write("The witch blocks your way!\n"); 
        return 1;
    }
    call_other(this_player(),"move_player","down#players/ringer/cellar");
    return 1;
}

search(str){
    object ob;
    if (!str) str = "room";
    if (str=="room") {
        write("A careful search of the shack reveals a trap door in the "
+"floor!\n");
        say(call_other(this_player(),"query_name") + " finds a trap door leading down.\n");
        passage_open=1;
    }
    return 1;
}

ex_reset() {
    int i;
    passage_open=0;
    if (!witch) {
        witch = clone_object("obj/monster");
        call_other(witch,"set_name","witch");
        call_other(witch,"set_alias","witch");
        call_other(witch,"set_level",5);
        call_other(witch,"set_long","The witch is old and evil looking.\n");
        call_other(witch,"set_aggressive",1);
        call_other(witch,"set_chance",10);
        call_other(witch,"set_spell_mess1","The witch points at you and shrieks!");
        call_other(witch,"set_spell_mess2","The witch points and shrieks!");
        call_other(witch,"set_spell_dam",5);
        call_other(witch,"set_al",-750);
        move_object(witch,this_object());
        if (present("black stone")) {
            item = present("black stone");
            move_object(item,witch);
        }
        if (!item || !raven || !living(raven)) {
            item = clone_object("players/ringer/fam_item");
            call_other(item,"set_id","black stone");
            call_other(item,"set_alias","stone");
            call_other(item,"set_short","A black shiny stone");
            call_other(item,"set_long","The stone is translucent.  You find yourself wanting to gaze into it.\n");
            call_other(item,"set_value",200);
            call_other(item,"set_weight",1);
            move_object(item,witch);            
            raven = clone_object("players/ringer/familiar");
            call_other(raven,"set_name","black raven");
            call_other(raven,"set_alias","raven");
            call_other(raven,"set_short","A black raven");
            call_other(raven,"set_long","The raven is large and black.  He stares at you menacingly.\n");
            call_other(raven,"set_level",1);
            call_other(raven,"set_aggressive",0);
            call_other(raven,"set_control_ob",item);
            call_other(item,"set_familiar",raven);
            call_other(raven,"set_familiar_mess",
                       "Your eyes clear and you see the world through a new perspective.  You\n"
                       +"ruffle your feathers.  FEATHERS!");
            call_other(raven,"set_think",this_object());
            move_object(raven,this_object());
        }  
    }
}

think(ob) {
    int t;
    t=random(4)+1;
    if (t==1){
        tell_object(ob,
"The raven thinks to himself: I wonder if the old hag is playing with my\n"
+"mind again. But, somehow it seems different this time...\n");
        return 1;
    }
    if (t==2) {
        tell_object(ob,
"The raven thinks to himself: Hmm, the old hag sure has been working in\n"
+"cellar alot lately.\n");
        return 1;
    }
    if (t==3) {
        tell_object(ob,
"The raven thinks to himself: Worms.  I need some good worms.\n");
        return 1;
    }
    if (t==4) {
        tell_object(ob,
"The raven thinks to himself: Oh for a nap in my nest.\n");
        return 1;
    }
}
