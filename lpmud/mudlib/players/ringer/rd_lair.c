#include "../../room/std.h"

#undef EXTRA_RESET
#define EXTRA_RESET\
        my_reset();

ONE_EXIT("players/ringer/en_for9", "south",
	   "You are in a quiet shaded place in the forest",
"You are in a quiet shaded spot in the forest.  It seems that this place\n"
+"is frequented by something very large!\n",1)

object dragon,item;

my_reset() {
    call_other("players/ringer/magtr","ex_reset");
}

ex_reset() {
    int i;
    if (!item || !dragon || !living(dragon)) {
        item = clone_object("players/ringer/fam_item");
        call_other(item,"set_id","red stone");
        call_other(item,"set_alias","stone");
        call_other(item,"set_short","A translucent red stone");
        call_other(item,"set_long",
"The stone is beautiful. Deep inside light plays in its facets resembling\n"
+"little tongues of flame.\n");
        call_other(item,"set_value",2000);
        call_other(item,"set_weight",1);
        move_object(item,"players/ringer/magtr");            
        dragon = clone_object("players/ringer/familiar");
        call_other(dragon,"set_name","cu_dragon");
        call_other(dragon,"set_race","copper dragon");
        call_other(dragon,"set_alias","dragon");
        call_other(dragon,"set_short","A copper dragon");
        call_other(dragon,"set_long",
"The dragon is large and regal.  His scales resemble polished copper. He\n"
+"looks peaceful.\n");
        call_other(dragon,"set_level",10);
        call_other(dragon,"set_ac",5);
        call_other(dragon,"set_wc",10);
        call_other(dragon,"set_aggressive",0);
        call_other(dragon,"set_chance",35);
        call_other(dragon,"set_spell_mess1","Copper dragon breathes fire.");
        call_other(dragon,"set_spell_mess2","Copper dragon breathes fire.");
        call_other(dragon,"set_spell_dam",40);
        call_other(dragon,"set_control_ob",item);
        call_other(item,"set_familiar",dragon);
        call_other(dragon,"set_familiar_mess",
  "Your eyes clear and you see the world through a new perspective.  You\n"
                   +"smooth out your coppery scales.  SCALES!");
        call_other(dragon,"set_think",this_object());
        move_object(dragon,this_object());
    }  
}

think(ob) {
    int t;
    t=random(4)+1;
    if (t==1){
        tell_object(ob,
"The dragon thinks to himself: I wonder how the Platinum Dragon hides his\n"
+"lair so well.  I know its in this forest somewhere...\n");
        return 1;
    }
    if (t==2) {
        tell_object(ob,
"The dragon thinks to himself: Hmm, Mulf sure has some strange new spells.\n");
        return 1;
    }
    if (t==3) {
        tell_object(ob,
"The dragon thinks to himself: I'm hungry for groos.\n");
        return 1;
    }
    if (t==4) {
        tell_object(ob,
"The dragon thinks to himself: The Chromatic Dragon.  I'd hate to live in.\n"
+"her place.  Dark, dank, overlooking a shear cliff. Uhg.  Give me the \n"
+"enchanted forest anytime!\n");
        return 1;
    }
}
