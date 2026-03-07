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

ONE_EXIT("players/ringer/thicket", "south",
	   "You are in a sheltered lair",
"You are in a lair sheltered by a canopy of thick branches.  It seems to\n" 
+"have been recently occupied.\n",1)

object dog,item,gorn;
int pack,passage_open;

ex_init() {
    add_action("wave"); add_verb("wave");
    add_action("wave"); add_verb("use");
    add_action("west"); add_verb("west");
    passage_open = 0;
}

ex_long(){
    if (passage_open) write("There is a passage open to the west\n");
}

west(){
    if (!passage_open) return 0;
    if (present("gorn")) {
        write("The Gorn blocks your way!\n");
        return 1;
    }
    call_other(this_player(),"move_player","west#players/ringer/gt_room");
}

wave(str){
    object ob;
    ob = present(str, this_player());
    if (ob) {
        if (passage_open) { write("Nothing happens.\n"); return 1;}
        if (call_other(ob,"id", "carved stick")) {            
            passage_open=1;
            say(call_other(this_player(),"query_name") 
                            + " waves a carved stick.\n");
            say(
"The carved stick leaps from his hands and buries itself in branches to the\n"
+ "west, bursting into flames.  The fire burns quickly revealing a passage.\n"
);
            tell_object(this_player(),
"The carved stick leaps from your hands and buries itself in branches to the\n"
+ "west, bursting into flames.  The fire burns quickly revealing a passage.\n"
);
            destruct(ob);
        }
    }
    return 1;
}

ex_reset() {
    int i;
    passage_open=0;
    if (!gorn) {
        gorn = clone_object("obj/monster");
        call_other(gorn,"set_name","gorn");
        call_other(gorn,"set_alias","gorn");
        call_other(gorn,"set_level",5);
        call_other(gorn,"set_long","The Gorn is a reptilian humanoid.\n");
        call_other(gorn,"set_aggressive",1);
        call_other(gorn,"set_al",-75);
        move_object(gorn,this_object());
        item = clone_object("obj/treasure");
        call_other(item,"set_id","large ruby");
        call_other(item,"set_alias","ruby");
        call_other(item,"set_short","A large ruby");
        call_other(item,"set_value",75);
        call_other(item,"set_weight",2);
        move_object(item,gorn);
        item = clone_object("obj/weapon");
        call_other(item,"set_name","green machete");
        call_other(item,"set_alias","machete");
        call_other(item,"set_class",12);
        call_other(item,"set_short","A green machete");
        call_other(item,"set_value",250);
        call_other(item,"set_weight",3);
        move_object(item,gorn);
        call_other(gorn,"init_command","wield machete");
        item = clone_object("players/ringer/ring1");
        call_other(item,"set_name","turquoise ring");
        call_other(item,"set_alias","ring");
        call_other(item,"set_short","A turquoise ring");
        move_object(item,gorn);
        item = clone_object("players/ringer/mitem");
        call_other(item,"set_id","carved stick");
        call_other(item,"set_real_id","passage wand");
        call_other(item,"set_alias","stick");
        call_other(item,"set_alias2","wand");
        call_other(item,"set_short","A carefully carved stick");
        call_other(item,"set_value",5);
        call_other(item,"set_weight",1);
        move_object(item,gorn);
    }
    if (!dog || !living(dog)) {
        pack = 3;
        i=0;
        while (i<pack) {
          i = i+1;
          dog = clone_object("players/ringer/blink_dog");
          call_other(dog,"set_name","blink wolf");
          call_other(dog,"set_alias","wolf");
          call_other(dog,"set_level",3);
          call_other(dog,"set_long","The wolf doesn't run like normal, he seems to teleport around.\n");
          call_other(dog,"set_wc",2);
          call_other(dog,"set_aggressive",1);
          call_other(dog,"set_chance",35);
          call_other(dog,"set_spell_mess1","The wolf howls");
          call_other(dog,"set_spell_mess2","The wolf howls at you");
          call_other(dog,"set_spell_dam",5);
          call_other(dog,"set_al",-60);
          move_object(dog,this_object());
          item = clone_object("players/ringer/mitem");
          call_other(item,"set_id","ruby amulet");
          call_other(item,"set_real_id","ruby amulet");
          call_other(item,"set_alias","amulet");
          call_other(item,"set_short","A ruby amulet");
          call_other(item,"set_value",50);
          call_other(item,"set_weight",2);
          call_other(item,"set_wear","The amulet is too small.\n");
          move_object(item,dog);
      }
    }
}
