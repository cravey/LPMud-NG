#include "../../room/std.h"

#undef EXTRA_RESET
#define EXTRA_RESET\
        ex_reset();

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

ONE_EXIT("players/ringer/old_shack", "up",
	   "You are in the witch's cellar.",
"You are in an old cellar.  It smells of hideous potions and brews.\n"
+"It seems the old witch did much of her work here.\n",1)

object scrap, pot, item;

ex_init() {
    add_action("talk"); add_verb("say");
}

talk(str) {
    int c;
    object m,n;
    
    if (!pot) return 0;
    if (call_other(pot,"query_flag") != 1)
      return 0;
    if (str != "emosfen" && str != "Emosfen")
      return 0;
    if (present(pot)) {
        m = present("gold bracelet",pot);
        n = present("silver ring",pot);
        if (n && m) {
            say(call_other(this_player(),"query_name")+" says "+str+".\n");
            tell_object(this_player(),"You are engulfed in black putrid smoke!\n");
            say(call_other(this_player(),"query_name") + " is enveloped in smoke.  When the smoke clears he\nand the caldron are gone.\n");
            move_object(pot,"players/ringer/sp_lair");
            destruct(pot);
            tell_object(this_player()," you are transported somewhere!\n");
            move_object(this_player(),"players/ringer/dk_ruin");
            return 1;
        }
    }
}


ex_reset() {
    int i;
    if (!present("knurled staff")) {
        item = clone_object("players/ringer/spitem");
        call_other(item,"set_id","knurled staff");
        call_other(item,"set_real_id","staff of griffons");
        call_other(item,"set_charges",5);
        call_other(item,"set_spell",5);
        call_other(item,"set_object",this_object());
        call_other(item,"set_alias","staff");
        call_other(item,"set_short","A knurled staff");
        call_other(item,"set_value",50);
        call_other(item,"set_weight",1);
        move_object(item,this_object());
    }
    if (!pot) {
        pot = clone_object("players/ringer/pot");
        call_other(pot,"set_id","black caldron");
        call_other(pot,"set_alias","caldron");
        call_other(pot,"set_alias2","pot");
        call_other(pot,"set_short","A black smoking caldron");
        call_other(pot,"set_long","A large caldron filled with a dark fluid.\n");
        call_other(pot,"set_value",5);
        call_other(pot,"set_weight",100);
        move_object(pot,this_object());
    }
    if (!scrap) {
        scrap = clone_object("players/ringer/mitem");
        call_other(scrap,"set_id","ripped page");
        call_other(scrap,"set_alias","paper");
        call_other(scrap,"set_alias2","page");
        call_other(scrap,"set_short","A ripped page");
        call_other(scrap,"set_long","The page appears to have a spell on it.\n");
        call_other(scrap,"set_read",
"It is some sort of transportation spell.  It reads:\n"
+"  In the caldron the spirits writhe,\n"
+"  Give them what they ask in tithe.\n"
+"  An enchanted braclet of purest gold,\n"
+"  A silver ring, fine and sleek,\n"
+"  Put in the pot by a raven's beak.\n"
+"  Then speak the words that you are told.\n");
        call_other(scrap,"set_value",5);
        call_other(scrap,"set_weight",1);
        move_object(scrap,this_object());
    }
}

 
use(str) {
    write(
"The staff glows white for a moment.  It makes a high pitched noise like\n"
+"that of a bird of prey.\n");
    say(
"The staff glows white for a moment.  It makes a high pitched noise like\n"
+"that of a bird of prey.\n");
    call_other(environment(this_player()),"call_grif");
    return 1;
}
