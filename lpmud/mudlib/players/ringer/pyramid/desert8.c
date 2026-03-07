#include "../../../room/std.h"

object gd1,item;
int passage;

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

#undef EXTRA_RESET
#define EXTRA_RESET ex_reset();

#undef EXTRA_LONG
#define EXTRA_LONG ex_long();

TWO_EXIT("players/ringer/pyramid/desert9","north",
          "players/ringer/pyramid/desert3","east",
          "A vast desolate desert",
"A desert stretches into the distance to the north and east.  To\n"+
"south the desert seems to end.  You feel a strange desert wind.\n",1)

ex_reset() {
    int i;
    if (!present("sand goblin")) 
      while (i<7) {
          gd1 = clone_object("obj/monster");
          call_other(gd1, "set_name", "sand goblin");
          call_other(gd1, "set_alias", "goblin");
          call_other(gd1, "set_level", 2);
          call_other(gd1, "set_al", -120);
          call_other(gd1, "set_short", "A sand goblin");
          call_other(gd1, "set_long",
"The goblin is short and dusty.  He looks stupid and belligerent.\n");
          call_other(gd1, "set_move_at_reset", 1);
          call_other(gd1, "set_aggressive", 1);
          if (random(10) > 5) {
              item = clone_object("obj/weapon");
              call_other(item,"set_name","short club");
              call_other(item,"set_alias","club");
              call_other(item,"set_class",5);
              call_other(item,"set_short","A short club");
              call_other(item,"set_value",80);
              call_other(item,"set_weight",2);
              move_object(item,gd1);
              call_other(gd1,"init_command","wield club");
          }
          move_object(gd1,this_object());
          i +=1;
      }
    passage=0;
}

ex_init() {
    if (random(100)>75 && !passage) {
        write(
"As you venture, an enormous windstorm arises.  You are temporarily\n"+
"blinded and a feeling of deep dread sweeps over you.  When the dust\n"+
"settles, you feel better.  You notice a large standstone building that\n"+
"you had not seen before.\n");
        say(
"An enormous windstorm arises.  You are temporarily blinded and a\n"+
"feeling of deep dread sweeps over you.  When the dust settles, you\n"+
"feel better.  You notice a large standstone building that you had\n"+
"not seen before.\n");
        passage=1;
    }
    add_action("enter"); add_verb("enter");
    add_action("search"); add_verb("search");
}

enter(str) {
    if (!passage) return 0;
    call_other(this_player(),"move_player",
               "entering building#players/ringer/pyramid/dquest");
    return 1;
}

ex_long() {
    if (passage) {
        write("A large sandstone building lies here.\n");
    }
}

search(str) {
    if (!str) str = "room";
    if (str == "room") {
        write("You find nothing unusual.\n");
        say(call_other(this_player(),"query_name") + " searches the room.\n");
        return 1;
    }
}





