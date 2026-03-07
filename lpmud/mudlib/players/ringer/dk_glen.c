#include "../../room/std.h"

#undef EXTRA_RESET
#define EXTRA_RESET\
        ex_reset();

TWO_EXIT("players/ringer/dk_trail", "east",
         "players/ringer/thicket","north", 
/*         "players/ringer/","west", */
/*         "players/ringer/","west", */
	   "You in a dark glen",
"The wood opens up into a glen here. The glen is small and surrounded by\n"
+"an incredibly dense wall of trees. The canopy over head is still thick\n"
+"enough to block out most sunlight. A low thicket lies to the north.\n",1)

object dog,item;
int pack;

ex_reset() {
    int i;
    if (!dog || !living(dog)) {
        pack = random(3)+3;
        i=0;
        while (i<pack) {
          i = i+1;
          dog = clone_object("players/ringer/blink_dog");
          call_other(dog,"set_name","blink dog");
          call_other(dog,"set_alias","dog");
          call_other(dog,"set_level",2);
          call_other(dog,"set_long","The dog doesn't run like normal, he seems to teleport around.\n");
          call_other(dog,"set_wc",2);
          call_other(dog,"set_move_at_reset",1);
          call_other(dog,"set_aggressive",1);
          call_other(dog,"set_chance",20);
          call_other(dog,"set_spell_mess1","The dog barks");
          call_other(dog,"set_spell_mess2","The dog barks at you");
          call_other(dog,"set_spell_dam",5);
          call_other(dog,"set_al",-50);
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
