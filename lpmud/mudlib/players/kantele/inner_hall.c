#include "std.h"
int shieldi;

object james;
#undef EXTRA_MOVE1
#define EXTRA_MOVE1\
    if (call_other(this_player(), "query_level", 0) < 20) {\
    if (james && present(james, this_object())) {\
        write("James bars your way !\n");\
        write("James says: Can you please leave, Sir !\n");\
        return 1;\
   }\
 }
#undef EXTRA_MOVE3
#define EXTRA_MOVE3\
    if (call_other(this_player(), "query_level", 0) < 20) {\
    if (james && present(james, this_object())) {\
        write("James bars your way !\n");\
        write("James says: Can you please leave, Sir !\n");\
        return 1;\
   }\
   }
#undef EXTRA_MOVE4
#define EXTRA_MOVE4\
    if (call_other(this_player(), "query_level", 0) < 20) {\
    if (james && present(james, this_object())) {\
        write("James bars your way !\n");\
        write("James says: Can you please leave, Sir !\n");\
        return 1;\
   }\
   }


#undef EXTRA_RESET
#define EXTRA_RESET\
   if (!james  || !living(james)) {\
        shield = 0;\
      james  = clone_object("obj/monster");\
      call_other(james, "set_name", "james");\
      call_other(james, "set_level", 12);\
      call_other(james, "set_al", 12);\
      call_other(james, "set_short", "James the butler is here");\
      call_other(james, "set_wc", 10);\
      call_other(james, "set_alias","butler");\
           call_other(james, "add_money", random(1000));\
      move_object(james, this_object());\
       }

#undef EXTRA_INIT
#define EXTRA_INIT add_action("get_shield"); add_verb("pry");
object shield;

FOUR_EXIT("players/kantele/stairway","south",
          "players/kantele/hall", "north",
         "players/kantele/dining_room","west",
         "players/kantele/living_room","east",
         "The inner hall",
         "You are in the inner hall, the walls are lined with shields\n" +
         "It looks like the shields are loot from some sort of campaign.\n",1)

get_shield(arg) {
if (arg != "of shield") {
write("pry of what ?\n");
} else {
   if (!present("shield", this_player()) &&  shieldi < 5) {
        shieldi = shieldi +1;
        write("you pry off a shield.\n");
        shield = clone_object("obj/armour");
        call_other(shield, "set_ac",1);
        call_other(shield, "set_name", "shield");
        call_other(shield, "set_alias", "small shield");
        call_other(shield, "set_value", 70);
        call_other(shield, "set_short", "A small metal shield");
        call_other(shield, "set_weight",4);
        call_other(shield, "set_type", "shield");
        move_object(shield, this_player());
        return 1;
                                            }
   else {
        write("the shields are fastened to hard.\n");
        return 1;
        }
}
}
